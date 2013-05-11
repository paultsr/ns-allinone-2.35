#include <mannasim/accessPointApp.h>
#include "leachAgent.h"

static class LeachAgentClass : public TclClass
{
	public:
		LeachAgentClass() : TclClass("Agent/RCAgent/LeachAgent") {}

		TclObject* create(int, const char*const*)
		{
			return (new LeachAgent());
		}
} class_leach_agent;


void LeachAgent::recv(Packet* p, Handler*)
{
	hdr_cmn *hdr = HDR_CMN(p);
	hdr_rca *rca_hdr = HDR_RCA(p);

//	if (app_ && (rca_hdr->rca_link_dst() < 0 || rca_hdr->rca_link_dst() == ((AcessPointApp *) app_)->sensor_node()->nodeid()))

	packetMsg_ = rca_hdr->msg_type();
	distEst_ = rca_hdr->dist_est();

	if (packetMsg_ == LEACH_DATA)
	{
// DEBUG
//		printf("|Access point received at %lf, packet = %d, distance = %lf, packet_msg = %d, meta = %d\n, meta_size = %d\n",
//			Scheduler::instance().clock(),
//			p,
//			distEst_,
//			packetMsg_,
//			rca_hdr->meta(),
//			rca_hdr->meta_size());
//			
//		printf("=) %d %d\n", rca_hdr->rca_link_src(), rca_hdr->meta());
		((AccessPointApp *) app_)->process_data(rca_hdr->meta_size(), (AppData *) rca_hdr->meta());
	}

//	((AccessPointApp *) app_)->recv(packetMsg_, distEst_, rca_hdr->rca_link_dst(), hdr->size(), rca_hdr->meta(),
//               rca_hdr->meta_size(), rca_hdr->rca_mac_src(), rca_hdr->rca_link_src());

	Packet::free(p);
}

