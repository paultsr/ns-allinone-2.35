#include "leachApp.h"
#include <random.h>
#include <mannasim/battery.h>
#include <mannasim/leach/rca/rcagent.h>

/** TCL **************************************/

static class LeachAppClass : public TclClass
{
        public:
                LeachAppClass() : TclClass("Application/SensorBaseApp/CommonNodeApp/LeachApp") {}
 
                // Create a TCL Object (parameters starts in 4)
                TclObject * create(int argc, const char * const * argv)
                {
			if (argc != 7)
			{
				fprintf(stderr,
					"Incorrect sintax:\n"
					"  new LeachApp <nNodes> <nClusters> <maxDist>");
			}

                        return new LeachApp(
                                atoi(argv[4]),
                                atoi(argv[5]),
				atof(argv[6]));
                }
} leach_app_object;
 



/*********************************************/

LeachApp::LeachApp(int nNodes, int nClusters, double maxDist) : CommonNodeApp() // : SensorBaseApp()
{
	mac_			 = NULL;
	agent_                   = NULL;

	isCH_                    = 0;
	hasBeenCH_               = false;
	nextChangeTime_          = 0;
	round_                   = 0;
	dist_                    = 0;
	code_                    = 0;
	now_                     = 0;
	endFrmTime_              = 0;
	beginIdle_               = 0;
	beginSleep_              = 0;

	/* Although constructing here, can't determine
	 * sensedData's nodeid, because while constructing
	 * the sensed_node_ is not yet defined.
	 */
	sensedData_              = new SensedData();

	eventHandler_            = new LeachEventHandler(this);

	config_.numberNodes_     = nNodes;
	config_.desiredClusters_ = nClusters;
	config_.maxDist_         = bsDist_ = maxDist;

	initializeConfig();

	frameTime_               = config_.ssSlotTime_ * config_.numberNodes_;

	lstRndDelay_             = 0;

	listenADV_               = true;
	listenJOINREQ_           = false;
}

/* Initialize LEACH's configuration. */
void LeachApp::initializeConfig()
{
	#define HDR_SIZE 25		// Originalmente era 25

	config_.changeTimeIncrement_ = 10 * INITIAL_ENERGY;
	config_.rndAdvDelay_         = TxTime(HDR_SIZE + 4); 
	config_.ra_advTotal_         = 1 + config_.rndAdvDelay_ * (config_.desiredClusters_ * 4 + 1);
	config_.ra_join_             = 0.01 * config_.numberNodes_;
	config_.ra_delay_            = TxTime(HDR_SIZE + 4);
	config_.spreading_           = config_.desiredClusters_ + 1;
	config_.sigSize_             = 500; 
	config_.ssSlotTime_          = TxTime(config_.sigSize_ + HDR_SIZE) * config_.spreading_;
	config_.bsCode_              = 0;

	#undef HDR_SIZE
}

LeachApp::~LeachApp()
{
	if (sensedData_ != NULL)
		delete sensedData_;

	delete eventHandler_;
}

void LeachApp::start()
{
	if (agent_ == NULL)
	{
		char agentName[32];

		printf("Warning! LeachApp::start() => agent_ of %d is null! Creating a RCAgent!\n", sensor_node_->nodeid());

		sprintf(agentName, "__rcagent%d", sensor_node_->nodeid());

		agent_ = new RCAgent();
		agent_->name(agentName);

		Tcl::instance().enter(agent_);

		printf("%s attach %s", name(), agent_->name());
		Tcl::instance().evalf("%s attach %s", name(), agent_->name());
	}

	sensedData_->node_id() == sensor_node_->nodeid();

	mac_ = (MacSensor *) ((RCAgent *) agent_)->getMAC();
	mac_->node_num() = sensor_node_->nodeid();
	decideClusterHead();

	CommonNodeApp::start();
}

void LeachApp::goToSleep()
{
	((Battery *) sensor_node_->energy_model())->sleep();
}

void LeachApp::wakeUp()
{
	((Battery *) sensor_node_->energy_model())->wakeUp();
}

void LeachApp::setCode(int code)
{
	printf("%d is setting code to %d\n", sensor_node_->nodeid(), code);
	code_        = code;
	mac_->code() = code;
}

void LeachApp::setClusterHead()
{
	isCH_      = true;
	hasBeenCH_ = true;
	/* "... when a node decides to become a cluster-head,
	 * it chooses randomly from a list of spreading codes."
	 */
}

void LeachApp::unsetClusterHead()
{
	isCH_      = false;
}

/*** Distributed cluster set-up functions *****************/

void LeachApp::decideClusterHead()
{
	int totRounds;

	setCode(0);
	wakeUp();

	CHHeard_  = false;

	// CheckIsAlive???

	totRounds = config_.numberNodes_ / config_.desiredClusters_;

	if (round_ >= totRounds)
		round_ = 0;	

	if (round_ == 0)
		setHasNotBeenClusterHead();

	if (Random::uniform(0, 1) < calculatePi())
	{
		printf("Node %d is a cluster head at time %lf\n", sensor_node_->nodeid(), Scheduler::instance().clock());

		setClusterHead();

		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::advertiseClusterHead),
			config_.rndAdvDelay_);
	}
	else
	{
		unsetClusterHead();
		listenADV_ = true;
		clearClusterChoices();
	}

	round_++;
	nextChangeTime_ = Scheduler::instance().clock() + config_.changeTimeIncrement_;

	Scheduler::instance().schedule(
		eventHandler_,
		new LeachEvent(&LeachApp::decideClusterHead),
//		nextChangeTime_);
		config_.changeTimeIncrement_);

	Scheduler::instance().schedule(
		eventHandler_,
		new LeachEvent(&LeachApp::findBestCluster),
		config_.ra_advTotal_);
}

double LeachApp::calculatePi()
{
	/*
	 * Pi(t) = k / (N - k mod(r,N/k))
	 * where k is the expected number of clusters per round
	 * N is the total number of sensor nodes in the network
	 * and r is the number of rounds that have already passed.
	 */
	register int n = config_.numberNodes_;
	register int k = config_.desiredClusters_;
	double thresh;

	if (hasBeenClusterHead())
		thresh = 0;

	else if (n - k * round_ < 1)
		thresh = 1;

	else
		thresh = (double) k / (n - k * round_);

	return thresh;
}

void LeachApp::advertiseClusterHead()
{
	int clusterCode;
	int numCodesAvail;

	numCodesAvail = 2 * config_.spreading_ - 1;

	
	currentCH_ = sensor_node_->nodeid();
	currentCHMAC_ = MAC_BROADCAST;

	clusterCode = (mac_->myADVnum() % numCodesAvail) + 1;

	setCode(clusterCode);

	wakeUp();

	send(
		MAC_BROADCAST,
		LINK_BROADCAST,
		LEACH_ADV_CH,
//		(char *) (&currentCH_),
		(char *) (&code_),
//		(char *) (&mac_->myADVnum()),
		sizeof(currentCH_),
		BYTES_ID,
		config_.maxDist_,
		0);

	listenJOINREQ_ = true;

	clusterNodes_.clear();
}

void LeachApp::findBestCluster()
{
//	int numCodesAvail, clusterCode;

//	numCodesAvail = 2 * config_.spreading_ - 1;

	if (isClusterHead())
	{
		// If node is CH, determine code and create a TDMA schedule.
		dist_      = config_.maxDist_;
		currentCH_ = sensor_node_->nodeid();
		currentCHMAC_ = MAC_BROADCAST;
//		myADVnum_  = mac_->myADVnum();

		/* There are opt(spreading) - 1 codes available b/c need 1 code
		 * for communication with the base station.
		 */
//		clusterCode   = (myADVnum_ % numCodesAvail) + 1;

		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::createSchedule),
			config_.ra_advTotal_ + config_.ra_join_);
	}
	else
	{
		int clusterCode;

		/* If node is not a CH, find the CH which allows minimum transmit
		 * power for communication.  Set the code and "distance" parameters
		 * accordingly.
		 */
		if (clusterChoices_.empty())
		{
			printf("Warning! No Cluster Head ADVs were heard by %d\n", sensor_node_->nodeid());
			currentCH_ = -1;	// VER ISSO *****
			currentCHMAC_ = MAC_BROADCAST;
			sendMyDataToBS();
			return;
		}

		double min_dist = config_.maxDist_ + 1;
		int ind = 0;

		for (CHs::iterator it = clusterChoices_.begin(); it != clusterChoices_.end(); it++, ind++)
		{
			chadv element = (chadv) *it;

			if (element.distance < min_dist)
			{
				min_dist    = element.distance;
				currentCH_  = element.nodeid;
				currentCHMAC_ = element.mac;
//				clusterCode = (ind % numCodesAvail) + 1;
				clusterCode = element.code;
			}
		}

		dist_ = min_dist;

		printf("%d has choosen %d as cluster head (mac = %d)\n", sensor_node_->nodeid(), currentCH_, currentCHMAC_);

		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::informClusterHead),
			config_.ra_advTotal_ + Random::uniform(0, config_.ra_join_ - config_.ra_delay_));;

		goToSleep();
	

		setCode(clusterCode);
		
		printf("Current cluster-head of %d is %d, which code is %d, at distance is %lf\n",
			sensor_node_->nodeid(),
			currentCH_,
			clusterCode,
			dist_);
	}

	listenADV_ = false;
	clearClusterChoices();
}

void LeachApp::informClusterHead()
{
	int dataSize;
	int nodeId;

	printf("%d sending JOIN_REQ to %d, distance = %lf , at time %lf\n",
		sensor_node_->nodeid(),
		currentCH_,
		dist_,
		Scheduler::instance().clock());

	dataSize = config_.spreading_ * BYTES_ID;
	nodeId   = sensor_node_->nodeid();

	send(
//		MAC_BROADCAST,
		currentCHMAC_,
		currentCH_,
		LEACH_JOIN_REQ,
		(char *) (&nodeId),
		sizeof(int),
		dataSize,
		config_.maxDist_,		// Using maxDist_, the CH can discover node's distance
//		dist_,
		code_);
}

void LeachApp::createSchedule()
{
	if (clusterNodes_.empty())
	{
		printf("Warning! There are no nodes in cluster %d\n",
			sensor_node_->nodeid());
		sendMyDataToBS();
	}
	else
	{
		int * msg;
		int i = 0;
		int dataSize;

		msg = new int[clusterNodes_.size()];

		printf("%d sending TDMA schedule (ADV_SCH): ", sensor_node_->nodeid());

		for (CNs::iterator it = clusterNodes_.begin(); it != clusterNodes_.end(); it++)
		{
			msg[i++] = (int) *it;
			printf("%d ", (int) *it);
		}

		printf("at time %lf\n", Scheduler::instance().clock());

		dataSize = config_.spreading_ * sizeof(int) * clusterNodes_.size();

		tdmaSchedule_.assign(clusterNodes_.begin(), clusterNodes_.end());

		send(
			MAC_BROADCAST,
//			sensor_node_->nodeid(),
			LINK_BROADCAST,
			LEACH_ADV_SCH,
			(char *) msg,
			sizeof(int) * clusterNodes_.size(),
			dataSize,
			dist_,
			code_);

		listenJOINREQ_ = false;

//		delete [] msg;

		frameTime_   = (5 + clusterNodes_.size()) * config_.ssSlotTime_;
		lstRndDelay_ = Random::uniform(0, 0.01);
		xmitTime_    = config_.ssSlotTime_ * (clusterNodes_.size()) + lstRndDelay_;

		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::sendDataToBS),
			xmitTime_);
	}
}

/*********************************************************/

void LeachApp::recv(int type, double distance, int link_dst, int size, char * meta, int meta_size, int src_mac, int src_lnk)
{
#ifdef CHATO
	printf("Receiving packet at %lf => ", Scheduler::instance().clock());
#endif

//	if (link_dst < 0) // || link_dst == sensor_node_->nodeid())
		switch (type)
		{
			case LEACH_ADV_CH:
#ifdef CHATO
				printf("LEACH_ADV_CH\n");
#endif
//				if (isClusterHead())
					recvADV_CH(meta, meta_size, distance, src_mac, src_lnk);
				break;

			case LEACH_JOIN_REQ:
#ifdef CHATO
				printf("JOIN_REQ\n");
#endif

//				if (sensor_node_->nodeid() == link_dst)
					recvJOIN_REQ(meta, meta_size);
				break;

			case LEACH_ADV_SCH:
#ifdef CHATO
				printf("LEACH_ADV_SCH\n");
#endif
				recvADV_SCH(meta, meta_size, src_mac);
				break;

			case LEACH_DATA:
#ifdef CHATO
				printf("LEACH_DATA\n");
#endif
//				if (sensor_node_->nodeid() == link_dst)
					recvData(meta, meta_size);
				break;

			case LEACH_BS_HELLO:
#ifdef CHATO
				printf("LEACH_BS_HELLO\n");
#endif
				recvBSHello(meta, meta_size, distance);
				break;

			default:
				fprintf(stderr, "Unknown received data type on LeachApp!\n", type);
				exit(-1);
		}
//	else
//		fprintf(stderr, "Received a packet addressed to another node!\n");
}

void LeachApp::recvADV_CH(char * msg, int size, double distance, int src, int lnk_src)
{
	if (listenADV_)
	{
		chadv element;

		if (!isClusterHead())
			printf("%d received ADV_CH from %d (mac = %d, distance = %lf, code = %d) at %lf\n",
				sensor_node_->nodeid(),
				lnk_src,
				src,
				distance,
				*((int *) msg),
				Scheduler::instance().clock());

		element.nodeid   = lnk_src;
		element.distance = distance;
		element.mac      = src;
		element.code     = *((int *) msg);
		element.object   = NULL;

		clusterChoices_.push_back(element);
	}
}

void LeachApp::recvJOIN_REQ(char * msg, int size)
{
	if (!isClusterHead())
	{
		fprintf(stderr, "Node %d received a JOIN_REQ from %d but it's not a cluster head!\n",
			sensor_node_->nodeid(),
			*((int *) msg));
		exit(-1);
	}

	if (listenJOINREQ_)
	{
		printf("%d received JOIN_REQ from %d at %lf\n",
			sensor_node_->nodeid(),
			*((int *) msg),
			Scheduler::instance().clock());

		clusterNodes_.push_back(*((int *) msg));
	}
	else
		printf("%d received a late JOIN_REQ from %d at %lf\n",
			sensor_node_->nodeid(),
			*((int *) msg),
			Scheduler::instance().clock());
}

void LeachApp::recvADV_SCH(char * msg, int size, int src)
{
	int * vector = (int *) msg;
	int elements = size / sizeof(int);
	int nodeid   = sensor_node_->nodeid();
	double xmitat;

	if (src == currentCHMAC_)
	{
		printf("%d received ADV_SCH from %d at %lf\n",
			sensor_node_->nodeid(),
			src,
			Scheduler::instance().clock());

		for (int i = 0; i < elements; i++)
			if (vector[i] == nodeid)
			{
				/* Determine time for a single TDMA frame. Each node sends data once
				 * per frame in the specified slot.
				 */
				frameTime_  = (5 + elements) * config_.ssSlotTime_;
				xmitTime_   = config_.ssSlotTime_ * i;
				endFrmTime_ = frameTime_ - xmitTime_;
				xmitat      = Scheduler::instance().clock() + xmitTime_;
	
				printf("Node %d schedule to transmit at %lf (%lf) | It is now %lf\n",
					nodeid,
					xmitat,
					xmitTime_,
					Scheduler::instance().clock());
	
				if (xmitat + endFrmTime_ < nextChangeTime_ - 10 * config_.ssSlotTime_)
				{
					Scheduler::instance().schedule(
						eventHandler_,
						new LeachEvent(&LeachApp::sendData),
						xmitTime_);

					goToSleep();
				}
				else
					printf("teste!!!!!!!\n");
				return;
			}

		// There is no time slot available
		printf("Warning!!! %d doesn't have a transmit time for CH %d!\n", sensor_node_->nodeid(), currentCH_);
	
		sendMyDataToBS();
	}
}

void LeachApp::recvData(char * msg, int size)
{
	printf("CH %d received data from %d at %lf",
		sensor_node_->nodeid(),
		((SensedData *) msg)->node_id(),
		Scheduler::instance().clock());

	mergeSensedData((SensedData *) msg);

	printf(" - Now there is %d sensed data\n",
		sensedData_->getData().size());
}

/* Occurs when received a base station "hello".
 * This is usefull to determine the BS distance from
 * node, but it is not original from LEACH paper.
 * -- J�lio C�sar e Melo
 */
void LeachApp::recvBSHello(char * msg, int size, double distance)
{
	bsDist_ = distance;

	printf("Node %d received BS_Hello at distance %lf\n",
		sensor_node_->nodeid(),
		distance);
}

/*********************************************************/

void LeachApp::sendData()
{
	int    dataSize;
	double xmitat;

	if (sensedData_->count() > 0 && !isClusterHead())
	{
		dataSize = config_.spreading_ * (BYTES_ID * sensedData_->msgSize() + config_.sigSize_);

		wakeUp();

		printf("Node %d sending %d data to CH %d at %lf | d = %lf\n",
			sensor_node_->nodeid(),
			sensedData_->count(),
			currentCH_,
			Scheduler::instance().clock() + frameTime_,
			dist_);

		sensedData_->node_id() = sensor_node_->nodeid();

		send(
	//		MAC_BROADCAST,
			currentCHMAC_,
			currentCH_,
			LEACH_DATA,
			sensedData_,
			dataSize,
			dist_,
			code_);

		// sensedData_ = new SensedData();
		clearSensedData();

		if (!isClusterHead())
			goToSleep();
	}

	xmitat = Scheduler::instance().clock() + frameTime_;

	if (xmitat + endFrmTime_ < nextChangeTime_ - 10 * config_.ssSlotTime_)
		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::sendData),
			frameTime_);
}

void LeachApp::send(int mac_dst, int link_dst, int type, char * msg, int msg_size, int data_size, double dist, int code)
{
	if (dist <= 0)
	{
		printf("Distancia invalida! %lf\n", dist);
		exit(-1);
	}

	((RCAgent *) agent_)->sendmsg(
		data_size,
		msg,
		msg_size,
		mac_dst,
		link_dst,
		dist,
		code,
		type);
}

void LeachApp::send(int mac_dst, int link_dst, int type, void * msg, int msg_size, int data_size, double dist, int code)
{
	send(
		mac_dst,
		link_dst,
		type,
		(char *) msg,
		msg_size,
		data_size,
		dist,
		code);
}

void LeachApp::send(int mac_dst, int link_dst, int type, SensedData * msg, int data_size, double dist, int code)
{
	send(
		mac_dst,
		link_dst,
		type,
		(char *) msg,
		msg->msgSize(),
		data_size,
		dist,
		code);
}

void LeachApp::sendDataToBS()
{
	int    dataSize, compressedSize;
	double dist;
	double xmitat;
	double rndDelay;

	rndDelay     = Random::uniform(0, 0.01);
	xmitat       = rndDelay - lstRndDelay_ + frameTime_;
	lstRndDelay_ = rndDelay;

	if (sensedData_->count() > 0)
	{
		dataSize       = config_.spreading_ * (BYTES_ID * sensedData_->msgSize() + config_.sigSize_);

		if (!isClusterHead())
			wakeUp();

		printf("Node %d sending %d data to BS at time %lf\n",
			sensor_node_->nodeid(),
			sensedData_->count(),
			Scheduler::instance().clock());

		sensedData_->timeStamp() = Scheduler::instance().clock();
		sensedData_->node_id() = sensor_node_->nodeid();

		send(
			MAC_BROADCAST,
			destination_id_,
			LEACH_DATA,
			sensedData_,
			dataSize,
			bsDist_,
			config_.bsCode_);

		if (!isClusterHead())
			goToSleep();

		clearSensedData();
	}
	
	if (xmitat + endFrmTime_ < nextChangeTime_ - 10 * config_.ssSlotTime_)
		Scheduler::instance().schedule(
			eventHandler_,
			new LeachEvent(&LeachApp::sendDataToBS),
			xmitat);
}

void LeachApp::sendMyDataToBS()
{
	sendDataToBS();
}

void LeachApp::disseminateData(SensedData * data)
{                                                                                                                            
	if (data != NULL)
	{
		printf("Common Node %d - Disseminating data -  Time %.3lf\n",
                        sensor_node_->nodeid(),Scheduler::instance().clock());

//		printf("%d is aggregating sensed data -  Time %.3lf",
//			sensor_node_->nodeid(),Scheduler::instance().clock());

		mergeSensedData(data);

		printf(" - There is %d data aggregated.\n", sensedData_->count()); //sensedData_->getData().size());
	}
}

void LeachApp::mergeSensedData(SensedData * data)
{
	AppDataList list;

	list = data->getData();

	for (AppDataList::iterator it = list.begin(); it != list.end(); it++)
		sensedData_->insertNewData(*it);
}

void LeachApp::clearClusterChoices()
{
	for (CHs::iterator it = clusterChoices_.begin(); it != clusterChoices_.end(); it++)
	{
		chadv element = (chadv) *it;
		
		if (element.object != NULL)
			delete element.object;
	}
	
	clusterChoices_.clear();
}

void LeachApp::clearSensedData()
{
	sensedData_->clear();
}

