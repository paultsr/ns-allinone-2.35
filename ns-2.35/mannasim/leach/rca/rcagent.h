/*************************************************************************
 *
 * This code was developed as part of the MIT SPIN project. (June, 1999)
 *
 *************************************************************************/


#ifndef ns_rca_h
#define ns_rca_h

#include "math.h"
#include "object.h"
#include "agent.h"
#include "trace.h"
#include "packet.h"
#include "priqueue.h"
#include "mac.h"
#include "random.h"

#include "agent.h"
#include "app.h"

//#include "../../sensorBaseApp.h"

#define SAMPLERATE 8000
#define ADV 0
#define REQ 1
#define DATA 2
#define RESEND 3

class RCAgent : public Agent {
public:
  RCAgent();
  ~RCAgent();
  inline void sendmsg(int data_size, const char* meta_data, int destination, int sendto, double dist_to_dest, int code, int packetMsg);
  void sendmsg(int data_size, const char* meta_data, int meta_size, int mac_dst, int link_dst, double dist_to_dest, int code, int packetMsg);

  virtual void recv(Packet*, Handler*);
  void log(const char *msg);
  int command(int argc, const char*const* argv);

protected:
  int packetMsg_;
  int packetSize_;
  double distEst_;

private:

  NsObject *ll;            // our link layer object 
  Mac *mac;    // our MAC layer object

  Trace *log_target;  // log target

public:
  inline Mac * getMAC() { return mac; }
};

#endif



