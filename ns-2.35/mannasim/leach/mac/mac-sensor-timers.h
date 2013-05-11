/******************************************************************
 * This code was developed as part of the MIT uAMPS project.      *
 ******************************************************************/

#ifndef __mac_sensor_timers_h__
#define __mac_sensor_timers_h__

/* ======================================================================
   Timers
   ====================================================================== */
class MacSensor;

class MacSensorTimer : public Handler {
public:
	MacSensorTimer(MacSensor* m, double s = 0) : mac(m) {
		busy_ = paused_ = 0; stime = rtime = 0.0; slottime_ = s;
	}

	virtual void handle(Event *e) = 0;

	virtual void start(Packet *p, double time);
	virtual void stop(Packet *p);
	virtual void pause(void) { assert(0); }
	virtual void resume(void) { assert(0); }

	inline int busy(void) { return busy_; }
	inline int paused(void) { return paused_; }
	inline double slottime(void) { return slottime_; }
	inline double expire(void) {
		return ((stime + rtime) - Scheduler::instance().clock());
	}

protected:
	MacSensor 	*mac;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
	double		slottime_;
};


class DeferSensorTimer : public MacSensorTimer {
public:
	DeferSensorTimer(MacSensor *m, double s) : MacSensorTimer(m,s) {}
	void	handle(Event *e);
};

class RxSensorTimer : public MacSensorTimer {
public:
	RxSensorTimer(MacSensor *m) : MacSensorTimer(m) {}

	void	handle(Event *e);
};

class TxSensorTimer : public MacSensorTimer {
public:
	TxSensorTimer(MacSensor *m) : MacSensorTimer(m) {}

	void	handle(Event *e);
};

#endif /* __mac_sensor_timers_h__ */

