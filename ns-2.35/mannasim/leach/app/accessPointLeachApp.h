#ifndef _ACCESSPOINTLEACHAPP_H_
#define _ACCESSPOINTLEACHAPP_H_

#include "../../accessPointApp.h"

/* AccessPointLeachApp
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * This is a class for access point using leach.
 */
class AccessPointLeachApp : public AccessPointApp {

	public:
		// Method called from the transport protocol for the application to 
		// receive a message. Method from Process (ns-process.cc) overcharged
//		virtual void process_data(int size, AppData* data);

//		int command(int argc, const char*const* argv);
//		virtual void stop();
		virtual void start();

		virtual void forward_data(int size, AppData* data);	
};


#endif
