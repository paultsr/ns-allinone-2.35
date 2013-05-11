#ifndef _LEACH_AGENT_H_
#define _LEACH_AGENT_H_

#include <mannasim/leach/app/leachApp.h>
#include <mannasim/leach/rca/rcagent.h>

class LeachAgent : public RCAgent
{
	public:
		virtual void recv(Packet*, Handler*);
};

#endif



