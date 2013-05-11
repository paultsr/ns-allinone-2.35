#ifndef _LEACHAPP_H_
#define _LEACHAPP_H_

#include <mannasim/sensorBaseApp.h>
#include <mannasim/sensedData.h>
#include <scheduler.h>
#include <list>
#include <mannasim/leach/mac/mac-sensor.h>

#define LEACH_ADV_CH           0
#define LEACH_JOIN_REQ         1
#define LEACH_ADV_SCH          2
#define LEACH_DATA             3
#define LEACH_BS_HELLO         4	/* Base station sends a "hello"
					 * to sensor nodes discover its distance.
					 * -- J�lio C�sar e Melo
					 */

#define BYTES_ID	 sizeof(int)

#define LINK_BROADCAST        -1

#define INITIAL_ENERGY 10
//#include <list>

//using namespace std;

#include <mannasim/commonNodeApp.h>

class LeachApp : public CommonNodeApp //SensorBaseApp
{
	protected:
		class LeachEvent : public Event
		{
			public:
				typedef void (LeachApp::*delegation) (void);

			private:
				delegation delegate_;

			public:
				LeachEvent(delegation delegate)
				{
					this->delegate_ = delegate;
				}

				delegation getDelegate() { return delegate_; }
				void setDelegate(delegation delegate) { delegate_ = delegate; }

				inline void executeDelegation(LeachApp * app)
				{
					(app->*delegate_)();
				}
		};

		class LeachEventHandler : public Handler
		{
			private:
				LeachApp * app_;
			public:
				LeachEventHandler(LeachApp * app) { app_ = app; }

				void handle(Event * event)
				{
					LeachEvent * levent = (LeachEvent *) event;

					levent->executeDelegation(app_);
				}
		};

	public:
		struct chadv
		{
			int    nodeid;
			int    mac;
			double distance;
			int    code;
			void * object;
		};

		typedef list <chadv> CHs;
		typedef list <int>   CNs;

	protected:
		LeachEventHandler     * eventHandler_;

		// List of cluster heads
		CHs                     clusterChoices_;

		// List of cluster nodes
		CNs			clusterNodes_;
		CNs			tdmaSchedule_;

		// Sensed data
		SensedData            * sensedData_;

	protected:
		MacSensor * mac_;
		double      nextChangeTime_;
	
	private:
		bool        isCH_;	        // Is cluster-head?
		bool        hasBeenCH_;      // Has been cluster-head?

	public:
		struct leachConfig
		{
			int         numberNodes_;		// opt(nn)
			int         desiredClusters_;		// opt(num_clusters)
			double	    changeTimeIncrement_;	// opt(ch_change)
			double      rndAdvDelay_;		// opt(ra_adv)
			double      ra_advTotal_;		// opt(ra_adv_total)
			double      ra_join_;			// opt(ra_join)
			double      ra_delay_;			// opt(ra_delay);
			double      maxDist_;			// opt(max_dist)
			int         spreading_;			// opt(spreading)
			double      ssSlotTime_;		// opt(ss_slot_time)
			int         sigSize_;			// opt(sig_size)	Bytes for data signal
			int         bsCode_;			// opt(bsCode)
		};

	protected:
		leachConfig config_;
		int         round_;
		double      dist_;
		double      bsDist_;
		int         code_;
		double      now_;
		double      frameTime_;
		double      endFrmTime_;
		double      beginIdle_;
		double      beginSleep_;
		int         myADVnum_;
		bool        CHHeard_;
		double      xmitTime_;
		double      lstRndDelay_;		// Last random delay
		int         currentCH_;
		int         currentCHMAC_;
		bool        listenADV_;			// If it's listenning to ADV
		bool        listenJOINREQ_;		// If it's listenning to JOIN_REQ
		
	public:
		LeachApp(int nNodes, int nClusters, double maxDist);
		~LeachApp();

		void start();

		void goToSleep();
		void wakeUp();

		// checkAlive not implemented!

		bool isClusterHead() { return isCH_; }
		bool hasBeenClusterHead() { return hasBeenCH_; }
		void setHasNotBeenClusterHead() { hasBeenCH_ = 0; }
		void setClusterHead();
		void unsetClusterHead();

		virtual void findBestCluster();
		virtual void informClusterHead();
		virtual void createSchedule();
		virtual void advertiseClusterHead();

		void recv(int type, double distance, int link_dst, int size, char * meta, int meta_size, int src_mac, int src_lnk);
		virtual void sendData();
		virtual void sendDataToBS();
		void sendMyDataToBS();

	protected:
		virtual void decideClusterHead();

		// Probability to turn cluster head
		virtual double calculatePi();

		virtual void recvADV_CH(char *, int, double, int, int);
		inline void recvJOIN_REQ(char *, int);
		inline void recvADV_SCH(char *, int, int);
		inline void recvData(char *, int);
		inline void recvBSHello(char *, int, double);

		inline void send(int mac_dst, int link_dst, int type, char * msg, int msg_size, int data_size, double dist, int code);
		inline void send(int mac_dst, int link_dst, int type, void * msg, int msg_size, int data_size, double dist, int code);
		inline void send(int mac_dst, int link_dst, int type, SensedData * msg, int data_size, double dist, int code);
        virtual void disseminateData(SensedData * data);

		/* Calculate time required to transmit n bytes of data,
		 * considering a 1 Mbps radio speed.
		 */
		virtual double TxTime(int n) { return n * 8.0 / 1000000.0; }

		/* Initialize LEACH's configuration */
		virtual void initializeConfig();

	protected:
		void setCode(int code);
	
	protected:
		virtual void mergeSensedData(SensedData *);
		void clearClusterChoices();
		virtual void clearSensedData();
		int calculateCompression(SensedData * sensedData);
};

#endif
