#include "accessPointLeachApp.h"
#include <mannasim/leach/leachAgent.h>

static class AccessPointLeachAppClass : public TclClass
{
	public:
		AccessPointLeachAppClass() : TclClass("Application/AccessPointApp/AccessPointLeachApp") {}

		TclObject* create(int, const char*const*)
		{
			return (new AccessPointLeachApp());
		}
} class_accesspointleachapp;



void AccessPointLeachApp::start()
{
	if (agent_ == NULL)
	{
		char agentName[32];

		printf("Warning! AccessPointLeachApp::start() => Agent is null! Creating a LeachAgent!\n");

		sprintf(agentName, "__apagent"); // N�o est� legal

		agent_ = new LeachAgent();
		agent_->name(agentName);

		Tcl::instance().enter(agent_);

		printf("%s attach %s", name(), agent_->name());
		Tcl::instance().evalf("%s attach %s", name(), agent_->name());
	}

	AccessPointApp::start();
}

void AccessPointLeachApp::forward_data(int size, AppData * data)
{
	// Por enquanto nada... ver compatibilidade depois ***
}
