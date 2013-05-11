/******************************************************************
 * This code was developed as part of the MIT uAMPS project.      *
 ******************************************************************/

#ifndef ns_mac_sensor_h
#define ns_mac_sensor_h

#include "mac.h"
#include "mac-sensor-timers.h"
#include "random.h"

#define GET_ETHER_TYPE(x)		GET2BYTE((x))
#define SET_ETHER_TYPE(x,y) {u_int16_t t = (y); STORE2BYTE(x,&t);}

#define ADV_TYPE 0

/* ======================================================================
   Frame Formats
   ====================================================================== */

struct hdr_macSensor {
	u_int16_t		dh_duration;
	u_char			dh_da[ETHER_ADDR_LEN];
	u_char			dh_sa[ETHER_ADDR_LEN];
	u_int16_t		dh_scontrol;
};


/* ======================================================================
   Definitions
   ====================================================================== */

#define ETHER_HDR_LEN				\
	(sizeof(struct hdr_macSensor) +		\
	 ETHER_FCS_LEN)

#define DATA_Time(len)	(8 * (len) / bandwidth_)


/* ======================================================================
   The following destination class is used for duplicate detection.
   ====================================================================== */

class Host {
public:
	LIST_ENTRY(Host) link;
	u_int32_t	index;
	u_int32_t	seqno;
};


/* ======================================================================
   The actual Sensor MAC class.
   ====================================================================== */

class MacSensor : public Mac {

	friend class DeferSensorTimer;
	friend class TxSensorTimer;
	friend class RxSensorTimer;

public:
	MacSensor();
	void		recv(Packet *p, Handler *h);

private:
	int		command(int argc, const char*const* argv);

	/*
	 * Packet Transmission Functions.
	 */
	void		send(Packet *p, Handler *h);

	/*
	 * Packet Reception Functions.
	 */
	void		recvDATA(Packet *p);
	void 		capture(Packet *p);
	void 		collision(Packet *p);

	inline int initialized() {
		return (cache_ && logtarget_ && Mac::initialized());
	}

	void mac_log(Packet *p) {
		logtarget_->recv(p, (Handler*) 0);
	}

	inline double TX_Time(Packet *p) {
		double t = DATA_Time((HDR_CMN(p))->size());
		if(t < 0.0) {
			Packet::free(p);
			exit(1);
		}
		return t;
	}

public:
	void  deferHandler(Event *e);
	void  recvHandler(Event *e);
	void  sendHandler(Event *e);

private:

	DeferSensorTimer	mhDefer_;
	TxSensorTimer			mhTx_;
	RxSensorTimer			mhRx_;

	int code_;       // the code this node is currently receiving
//      int base_X;      // the position X of the base station
//      int base_Y;      // the position Y of the base station
        int node_num_;   // this node's ID number
	int ss_;         // max number of overlapping ss transmissions
	int CHheard_;    // Number of other CH ADVs heard
	int myADVnum_;   // Position of node's ADV among all ADVs

	/* ============================================================
	   Internal MAC State
	   ============================================================ */

	MacState		rx_state_;	// incoming state (MAC_RECV or MAC_IDLE)
	MacState		tx_state_;	// outgoing state

	inline int	is_idle(void);

	NsObject*		logtarget_;

	/* ============================================================
	   Duplicate Detection state
	   ============================================================ */

	u_int16_t	sta_seqno_;		// next seqno that I'll use
	int				cache_node_count_;
	Host			*cache_;

public:
	inline int & node_num() { return node_num_; }
	inline int & code() { return code_; }
	inline int & myADVnum() { return myADVnum_; }

};


#endif /* __mac_sensor_h__ */

