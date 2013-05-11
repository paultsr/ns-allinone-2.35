/*************************************************************************
 *
 * This code was developed as part of the MIT SPIN project. (June, 1999)
 *
 *************************************************************************/


#ifndef rca_ll_h
#define rca_ll_h

#include "delay.h"
#include "queue.h"

#include "arp.h"
#include "node.h"
#include "god.h"

class RCALinkLayer : public LL {
    public:
        RCALinkLayer() : LL() {};
    private:
        virtual void  sendDown(Packet* p);
        virtual void  sendUp(Packet* p);
};

#endif



