/*************************************************************************
 *
 * This code was developed as part of the MIT SPIN project. (June, 1999)
 *
 *************************************************************************/


#include "delay.h"
//#include "object.h"
#include "packet.h"

//#include "debug.h"
#include <list>
#include "arp.h"
#include "topography.h"
#include "trace.h"
#include "node.h"
#include "mac.h"
#include "ll.h"
#include "mannasim/leach/rca/rca-ll.h"
#include "random.h"

static class RCALinkLayerClass : public TclClass {
    public:
        RCALinkLayerClass() : TclClass("RCALinkLayer") {}
        TclObject* create(int, const char*const*) {
            return (new RCALinkLayer);
        }
} class_rcalinklayer;

void RCALinkLayer::sendDown(Packet* p)
{  
    Scheduler& s = Scheduler::instance();
    hdr_rca *rcah = HDR_RCA(p);
    hdr_ll *llh = HDR_LL(p);
    char *mh = (char*)p->access(hdr_mac::offset_);

    llh->seqno() = ++seqno_;
    llh->lltype() = LL_DATA;

    mac_->hdr_src(mh, mac_->addr());
    mac_->hdr_type(mh, ETHERTYPE_IP);
    mac_->hdr_dst((char*) HDR_MAC(p), rcah->rca_mac_dst());

    //  Packet::PrintRcHeader(p,"Link Outgoing");

    //  mac_->hdr_dst((char*)HDR_MAC(p),rcah->rca_mac_dst());
    //s.schedule(downtarget_, p, mindelay_ / 2 + delay_ * Random::uniform());
    s.schedule(downtarget_, p, delay_);
}


void RCALinkLayer::sendUp(Packet* p)
{  
    Scheduler& s = Scheduler::instance();
    hdr_rca *rcah = HDR_RCA(p);

    //  Packet::PrintRcHeader(p,"Link Incomming");
    if (hdr_cmn::access(p)->error() > 0) {
        printf("Warning!  %d dropping packet from %d b/c it has an error.\n", rcah->rca_mac_dst(), rcah->rca_mac_src());
        fflush(stdout);
        drop(p);
    } else
        s.schedule(uptarget_, p, delay_);
}

