/* mannanmp.h
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * MannaNMPAgent is a class for answering
 * MannaNMP messages using directed diffusion.
 */
#ifndef _MANNANMP_H_
#define _MANNANMP_H_

#include "nrAttributes.h"
#include "../management/mibTable.h"
#include <nr.hh>

class MannaNMPAgent : public NR::Callback
{
	protected:
		MIBTable *   mib_;
		NR *         dr_;
		handle       hnd_;

	public:
		MannaNMPAgent(MIBTable * mib, NR * dr);
		void recv(NRAttrVec * data, handle h);
		void setHandle(handle h) { hnd_ = h; }

	private:
		void onSubscription(NRAttrVec * data, handle h);
		void onGetMessage(GetData * message, handle h);
		void onSetMessage(SetData * message, handle h);
};

#endif
