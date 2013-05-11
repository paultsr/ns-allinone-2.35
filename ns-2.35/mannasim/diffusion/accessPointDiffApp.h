#ifndef _ACCESSPOINTDIFFAPP_H_
#define _ACCESSPOINTDIFFAPP_H_

#include "nrAttributes.h"
#include "../sensorBaseApp.h"
#include "../temperatureAppData.h"

/* AccessPointDiffApp
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * This is a class for access point using directed diffusion.
 */
class AccessPointDiffApp : public DiffApp, NR::Callback
{
	public:
		AccessPointDiffApp();

		void run() {};
		virtual int command(int argc, const char * const * argv);

		// Receive message from directed diffusion
		void recv(NRAttrVec * data, NR::handle my_handle);

	protected:
		// Subscribe for an interest
		handle subscribe(AppDataType appDataType, NRAttribute::operators op, double value);
		
		// Receive sensed data
		virtual void recvSensedData(NRAttrVec *, NR::handle);
		virtual void recvSpecificSensedData(AppDataType type, NRAttrVec *, NR::handle);

	private:
		virtual void recvData(NRAttrVec * data, NR::handle my_handle);

		// Execute Tcl Management Commands
		int execTclSubscribe(int argc, const char * const *);
};

#endif
