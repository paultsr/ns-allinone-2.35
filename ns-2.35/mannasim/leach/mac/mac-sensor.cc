/******************************************************************
 * This code was developed as part of the MIT uAMPS project.      *
 ******************************************************************/

#include "mac-sensor.h"
#include "leach-wireless-phy.h"


/* ======================================================================
   Macros
   ====================================================================== */

#define SET_RX_STATE(x)                \
{                                      \
  rx_state_ = (x);                    \
}

#define SET_TX_STATE(x)                \
{                                      \
  tx_state_ = (x);                    \
}


/* ======================================================================
   TCL Hooks for the simulator
   ====================================================================== */
static class MacSensorClass : public TclClass {
public:
  MacSensorClass() : TclClass("Mac/Sensor") {}
  TclObject* create(int, const char*const*) {
    return (new MacSensor());
  }
} class_macSensor;


/* ======================================================================
   Mac Class Functions
   ====================================================================== */
MacSensor::MacSensor() : Mac(), mhDefer_(this, 0.005), mhTx_(this), 
  mhRx_(this), code_(0), ss_(10), CHheard_(0),
  myADVnum_(0)
{

  tx_state_ = rx_state_ = MAC_IDLE;

  sta_seqno_ = 1;
  cache_ = 0;
  cache_node_count_ = 0;

/*  bind("code_",&code_);
//  bind("base_X",&base_X);
//  bind("base_Y",&base_Y);
  bind("node_num_",&node_num_);
  bind("ss_",&ss_);
  bind("CHheard_",&CHheard_);
  bind("myADVnum_",&myADVnum_);
*/
}

int
MacSensor::command(int argc, const char*const* argv)
{
  if (argc == 3) {

    if (strcmp(argv[1], "log-target") == 0) {
      logtarget_ = (NsObject*) TclObject::lookup(argv[2]);
      if(logtarget_ == 0)
        return TCL_ERROR;
      return TCL_OK;
    }

    if(strcmp(argv[1], "nodes") == 0) {
      if(cache_) return TCL_ERROR;
      cache_node_count_ = atoi(argv[2]) + 1;
      cache_ = new Host[cache_node_count_ + 1];
      assert(cache_);
      bzero(cache_, sizeof(Host) * (cache_node_count_+1 ));
      return TCL_OK;
      
    }
  }
  return Mac::command(argc, argv);
}

/* ======================================================================
   Misc Routines
   ====================================================================== */

inline int
MacSensor::is_idle()
{
  if(rx_state_ != MAC_IDLE)
    return 0;

  if(tx_state_ != MAC_IDLE)
    return 0;

  return 1;
}

/* ======================================================================
   Outgoing Packet Routines
   ====================================================================== */

void
MacSensor::send(Packet *p, Handler *h)
{
  struct hdr_macSensor* dh = HDR_MACSensor(p);

  if (h != 0)
    callback_ = h;

  hdr_rca *rca_hdr = HDR_RCA(p);
  int code = rca_hdr->get_code();

  /* 
   * Perform carrier sence.  If the channel is busy, backoff. 
   */
  if(!is_idle() || ((LeachWirelessPhy *) netif_)->csEnd(code) > Scheduler::instance().clock()) {
    /*
    printf("CSMA: Node %d defering %s for time %f: code = %d ...\n", 
            rca_hdr->rca_src(),rca_hdr->meta(),TX_Time(p),code);
    fflush(stdout);
    */
    //mhDefer_.start(p, Random::uniform(TX_Time(p)));
    mhDefer_.start(p, TX_Time(p));
    return;
  }

  pktTx_ = p;

  /*
   * Determine how many ADV messages have been heard to determine
   * spreading code to use for each cluster.
   */
  myADVnum_ = CHheard_;

  hdr_cmn::access(p)->txtime() = TX_Time(p);

  /*
   * Assign the data packet a sequence number.
   */
  dh->dh_scontrol = sta_seqno_++;

  SET_TX_STATE(MAC_SEND);              
  /* 
   * Start a timer that expires when the packet transmission is complete.
   */
  mhTx_.start(p->copy(), TX_Time(p));
  downtarget_->recv(pktTx_, this);        
  /*
  struct hdr_mac *mh = HDR_MAC(pktTx_);
  printf("%d sending data to %d at time %f\n", mh->macSA(), mh->macDA(),
  Scheduler::instance().clock());
  fflush(stdout);
  */

  if(callback_) {
    Handler *h = callback_;
    callback_ = 0;
    h->handle((Event*) 0);
  } 

}

/* ======================================================================
   Incoming Packet Routines
   ====================================================================== */

void
MacSensor::recv(Packet *p, Handler *h)
{
  struct hdr_cmn *hdr = HDR_CMN(p);

  /*
   * Sanity Check
   */
  assert(initialized());

  /*
   * Handle outgoing packets.
   */
  if(hdr->direction() == -1) {

    /*
      * Update the MAC header
      */
    hdr->size() += ETHER_HDR_LEN;
    send(p, h);
    return;
  }

  /*
   * Handle incoming packets.
   * We just received the 1st bit of a packet on the interface.
   */

  hdr_rca *rca_hdr = HDR_RCA(p);
  if (rca_hdr->msg_type() == ADV_TYPE) 
    CHheard_++;

  /*
   * If I am not receiving the code of the incoming packet, drop it.
   */
  int new_code = rca_hdr->get_code();
  if (new_code != code_) {
      
//      printf("In MAC_IDLE %d: %d is not a code I am currently receiving %d.\n", node_num_, new_code, code_);
//      fflush(stdout);
      
        Packet::free(p);
      return;
  }

  /*
   *  If the interface is currently in transmit mode, then
   *  it probably won't even see this packet.  However, the
   *  "air" around me is BUSY so I need to let the packet
   *  proceed.  Just set the error flag in the common header
   *  so that the packet gets thrown away.
   */
  if(tx_state_ && hdr->error() == 0) {
    printf("Warning! %d: Cannot receive while transmitting.\n", node_num_);
    fflush(stdout);
    hdr->error() = 1;
  }

  /* 
   * If more than ss_ simultaneous transmissions occur, there is a
   * collision and I cannot receive the packet.
   */ 
  int num_codes = 0;
  for (int i = 0; i < 1000; i++) 
    if (((LeachWirelessPhy *) netif_)->csEnd(i) > Scheduler::instance().clock()) 
      num_codes++;
  if (num_codes > ss_) {
    printf("I can hear %d different packets ==> collision.\n", num_codes);
    fflush(stdout);
    collision(p);
    return;
  }

  if(rx_state_ == MAC_IDLE) {

    SET_RX_STATE(MAC_RECV);

    pktRx_ = p;

    /*
     * Schedule the reception of this packet, in txtime seconds.
     */
    double rtime = TX_Time(p);
    assert(rtime >= 0.0);
    mhRx_.start(p, rtime);
  }
  else {
    /*
     *  If the power of the incoming packet is smaller than the
     *  power of the packet currently being received by at least
     *  the capture threshold, then we ignore the new packet.
     */
  
    if(hdr->error() == 0) {
      hdr->error() = 1;
    }
     if(pktRx_->txinfo_.RxPr / p->txinfo_.RxPr >= p->txinfo_.CPThresh) {
        capture(p);
     } else {
      collision(p);
     }
  }
}

void
MacSensor::capture(Packet *p)
{
  printf("CAPTURE!!!!\n");
  fflush(stdout);
  Packet::free(p);
  //p = 0;
}

void
MacSensor::collision(Packet *p)
{
  printf("COLLISION!!!!\n");
  fflush(stdout);
  switch(rx_state_) {

  case MAC_RECV:
    SET_RX_STATE(MAC_COLL);

    /* fall through */

  case MAC_COLL:
    assert(pktRx_);
    assert(mhRecv_.busy);

    /*
     *  Since a collision has occurred, figure out
     *  which packet that caused the collision will
     *  "last" the longest.  Make this packet,
     *  pktRx_ and reset the Recv Timer if necessary.
     */
    if(TX_Time(p) > mhRx_.expire()) {
      mhRx_.stop(pktRx_);
      Packet::free(pktRx_);
      pktRx_ = p;
      mhRx_.start(pktRx_,TX_Time(pktRx_));
    }
    else {
      Packet::free(p);
    }
    break;

  default:
    assert(0);
  }
}

void
MacSensor::recvDATA(Packet *p)
{
  /*
   * Adjust the MAC packet size - ie, strip off the mac header.
   */
  struct hdr_cmn *ch = HDR_CMN(p);
  ch->size() -= ETHER_HDR_LEN;
  ch->num_forwards() += 1;

  /*
   *  Pass the packet up to the link-layer.
   */
  uptarget_->recv(p, (Handler*) 0);
}

void
MacSensor::deferHandler(Event *e)
{
  /*
  hdr_rca *rca_hdr = HDR_RCA((Packet *)e);
  printf("Node %d trying to send %s again at time %f.\n", node_num_, 
          rca_hdr->meta(), Scheduler::instance().clock());
  fflush(stdout);
  */
  /*
   * After timer expires, try to transmit the packet (i.e., perform
   * carrier sense again to see if the channel is now free).
   */
  send((Packet *)e,(Handler*) 0);
}

void
MacSensor::recvHandler(Event *e)
{

  if (rx_state_ == MAC_COLL) {
    struct hdr_cmn *ch = HDR_CMN(pktRx_);
    ch->error() = 1;
  }
  SET_RX_STATE(MAC_IDLE);

  /*
   * If this packet was unicast and not intended for me, drop it.
   */   
  struct hdr_mac *mh = HDR_MAC(pktRx_);
  int dst = mh->macDA();

  if (dst != index_ && (u_int32_t) dst != (u_int32_t) MAC_BROADCAST) 
      Packet::free(pktRx_);
  else 
    recvDATA(pktRx_);
}

void
MacSensor::sendHandler(Event *e)
{
  /* 
   * Once transmission is complete, drop the packet.
   */
  SET_TX_STATE(MAC_IDLE);
  Packet::free((Packet *)e);
}
