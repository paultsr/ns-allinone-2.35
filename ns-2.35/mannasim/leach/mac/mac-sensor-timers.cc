/******************************************************************
 * This code was developed as part of the MIT uAMPS project.      *
 ******************************************************************/

#include "mac-sensor.h"
#include "mac-sensor-timers.h"


/* ======================================================================
   Timers
   ====================================================================== */

void
MacSensorTimer::start(Packet *p, double time)
{
	Scheduler &s = Scheduler::instance();

	assert(busy_ == 0);

	busy_ = 1;
	paused_ = 0;
	stime = s.clock();
	rtime = time;
	assert(rtime >= 0.0);

	s.schedule(this, p, rtime);
}

void
MacSensorTimer::stop(Packet *p)
{
	Scheduler &s = Scheduler::instance();

	assert(busy_);

	if(paused_ == 0)
		s.cancel((Event *)p);

	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
}

/* ======================================================================
   Defer Timer
   ====================================================================== */

void    
DeferSensorTimer::handle(Event *e)
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;

	mac->deferHandler(e);
}

/* ======================================================================
   Receive Timer
   ====================================================================== */

void    
RxSensorTimer::handle(Event *e)
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;

	mac->recvHandler(e);
}


/* ======================================================================
   Send Timer
   ====================================================================== */

void    
TxSensorTimer::handle(Event *e)
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;

	mac->sendHandler(e);
}

