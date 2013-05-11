/* CommonNodeDiffApp.cc
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * Implementation of CommonNodeDiffApp.
 * It is a class for common sensor node, using
 * directed diffusion.
 */

#include "commonNodeDiffApp.h"
#include <common/ns-process.h>
#include <stdexcept>

// Provides an instance of the CommonNodeDiffApp class in the simulation script
static class CommonNodeDiffAppClass : public TclClass
{
	public:
		CommonNodeDiffAppClass() : TclClass("Application/SensorBaseApp/CommonNodeDiffApp") {}

		TclObject * create(int, const char * const *)
		{
			return new CommonNodeDiffApp::TclObjectAdaptor();
		}
} class_common_node_diff_app;

/**************************************************************************/

// Constructor
CommonNodeDiffApp::CommonNodeDiffApp()
{
}

// Destructor
CommonNodeDiffApp::~CommonNodeDiffApp()
{
	PublishedDataList::iterator it;

	for (it = publishedDataList_.begin(); it != publishedDataList_.end(); it++)
		dr_->unpublish((*it)->hnd);
}

// Overrides CommonNodeApp's function
void CommonNodeDiffApp::insertNewGenerator(DataGenerator * gen)
{
	CommonNodeApp::insertNewGenerator(gen);

	publishData(gen->type());
}

/* Overrides CommonNodeApp's function
 *
 * This is called to disseminateData from sensor node.
 */
void CommonNodeDiffApp::disseminateData(SensedData * data)
{
	if (data)
	{
		NRAttrVec       attrs;
		PublishedData * pubData;
		AppDataList     list;

		list = data->getData();

		printf("Common Node %d"
			" - Disseminating %d datas\n",
			sensor_node_->nodeid(),
			list.size());

		// Send each sensed data using directed diffusion
		for (AppDataList::iterator it = list.begin();
			it != list.end();
			it++)
		{
			pubData = publishData((*it)->type(), (AttrsExporter *) (*it));

			attrs.push_back(
				TimeStampAttr.make(NRAttribute::IS, Scheduler::instance().clock()));

/*			if (interested(&attrs))
			{
				printf("Common Node %d"
					" - Disseminating %d datas\n",
					sensor_node_->nodeid(),
					list.size());
			}

*/			if (dr_->send(pubData->hnd, &attrs) == FAIL)
			{
				fprintf(stderr, "bug! CommonNodeDiffApp::publishData\n");
				//abort();
                exit(1);
			}

			ClearAttrs(&attrs);
		}
	}
}

/* Find a handle to a published data generetador */
CommonNodeDiffApp::PublishedData * CommonNodeDiffApp::findPublishedData(AppDataType appDataType)
{
	PublishedDataList::iterator it;

	it = publishedDataList_.begin();

	while (it != publishedDataList_.end())
	{
		if ((*it)->appDataType == appDataType)
			return *it;

		it++;
	}

	fprintf(stderr,
		"Unable to find AppDataType for specific sensed data!\n");

	//throw runtime_error("AppDataType not found for specific sensed data!");
	//abort();
    exit(1);
}

/* Publish data.
 *
 * This is called for the first publication, which occurs
 * on data generator attachment.
 */
CommonNodeDiffApp::PublishedData * CommonNodeDiffApp::publishData(
	AppDataType appDataType,
	NR::Callback * receiver)
{
	NRAttrVec     attrs;
	PublishedData * my_publishedData;

	// Set attributes and publish it for data generator
	attrs.push_back(
		NRClassAttr.make(NRAttribute::IS, NRAttribute::DATA_CLASS));

	attrs.push_back(
		AppDataTypeAttr.make(NRAttribute::IS, appDataType));

	attrs.push_back(
		NodeIDAttr.make(NRAttribute::IS, this->sensor_node_->nodeid()));

	// Publish data
	my_publishedData                = new PublishedData;
	my_publishedData->hnd           = dr_->publish(&attrs);
	my_publishedData->appDataType   = appDataType;
	my_publishedData->subscriptions = 0;

	ClearAttrs(&attrs);

	publishedDataList_.push_back(my_publishedData);

	// Create subscription handler
	if (!receiver)
		my_publishedData->receiver = new InterestReceiver(
			this, my_publishedData);
	else
		my_publishedData->receiver = receiver;

	subscribePublishedData(my_publishedData);

	return my_publishedData;
}

/* Publish sensed data.
 *
 * This function searches for previous publishe data
 * with the same AppDataType and re-publish it,
 * containing a new sensed data value.
 *
 * This is usefull to access point subscribe for
 * conditional sensed data (i.e. temperature > 30).
 */
CommonNodeDiffApp::PublishedData * CommonNodeDiffApp::publishData(AppDataType appDataType, AttrsExporter * attrsExporter)
{
	NRAttrVec     attrs;
	PublishedData * my_publishedData;

	// Set attributes and publish it for data generator
	attrs.push_back(
		NRClassAttr.make(NRAttribute::IS, NRAttribute::DATA_CLASS));

	attrs.push_back(
		AppDataTypeAttr.make(NRAttribute::IS, appDataType));

	attrs.push_back(
		NodeIDAttr.make(NRAttribute::IS, this->sensor_node_->nodeid()));

	attrsExporter->setAttributes(&attrs);

	// Unpublish previous data
	my_publishedData                = findPublishedData(appDataType);

	dr_->unpublish(my_publishedData->hnd);

	my_publishedData->hnd           = dr_->publish(&attrs);
	my_publishedData->subscriptions = 0;

	ClearAttrs(&attrs);

	return my_publishedData;
}

/* subscribePublishedData(pubData)
 * - pubData: Subscribe to receive subscriptions and unsubscriptions
 */
void CommonNodeDiffApp::subscribePublishedData(PublishedData * pubData)
{
	NRAttrVec attrs;

	attrs.push_back(
		NRClassAttr.make(NRAttribute::NE, NRAttribute::DATA_CLASS));

	attrs.push_back(
		NRScopeAttr.make(NRAttribute::IS, NRAttribute::NODE_LOCAL_SCOPE));	

//	attrs.push_back(
//		AppDataTypeAttr.make(NRAttribute::EQ, pubData->appDataType));

//	attrs.push_back(
//		NodeIDAttr.make(NRAttribute::EQ, this->sensor_node_->nodeid()));

	dr_->subscribe(&attrs, pubData->receiver);

	ClearAttrs(&attrs);
}

void CommonNodeDiffApp::run()
{
	// Insert generic sensed data
	publishData(SENSED_DATA);
}

void CommonNodeDiffApp::start()
{
	DiffApp::start();
	CommonNodeApp::start();

	run();
}

/* * * Event handle * * */

// Occurs when someone subscribes
void CommonNodeDiffApp::onSubscription(PublishedData * pubData, NRAttrVec * data)
{
	NRSimpleAttribute<AppDataType> * appDataType;

	appDataType = AppDataTypeAttr.find(data);

	if (!appDataType)
		printf("Common Node %d received an Interest message!\n",
			sensor_node_->nodeid());

	else
		printf("Common Node %d received an Interest message for app data %d\n",
			sensor_node_->nodeid(), appDataType->getVal());

	subscriptions_.push_back(CopyAttrs(data));
}

void CommonNodeDiffApp::onUnsubscription(PublishedData * pubData, NRAttrVec * data)
{
	NRAVList::iterator it;

	printf("Common Node %d received an Disinterest message!\n",
		sensor_node_->nodeid());

	for (it = subscriptions_.begin(); it != subscriptions_.end(); it++)
		if (PerfectMatch(data, *it))
		{
			NRAttrVec * tmp = *it;

			subscriptions_.remove(*it);

			delete tmp;

			return;
		}
}

/*************************************************************************
 * Interest receiver class                                               *
 *                                                                       *
 * This class is used to handle Interest messages.                       *
 *************************************************************************/

CommonNodeDiffApp::InterestReceiver::InterestReceiver(CommonNodeDiffApp * node, PublishedData * pubData)
{
	this->node_    = node;
	this->pubData_ = pubData;
}

void CommonNodeDiffApp::InterestReceiver::recv(NRAttrVec * data, NR::handle hnd)
{
	NRSimpleAttribute<int> * nrClass;
	NRSimpleAttribute<AppDataType> * nrAppDataType;

	// Get message's class and validate it
	nrClass = NRClassAttr.find(data);

	if (!nrClass)
	{
		fprintf(stderr, "Received a bad diffusion packet!\n");
		//abort();
        exit(1);
	}

//	printf("AIEEEEEEEE!\n");

	nrAppDataType = AppDataTypeAttr.find(data);

//	assert(nrAppDataType && nrAppDataType->getVal() == pubData_->appDataType);

/*	if (!nrAppDataType)
	{
		fprintf(stderr, "Not a valid packet for InterestReceiver!\n");
		abort();
	}
	else if (nrAppDataType->getVal() != pubData_->appDataType)
	{
		fprintf(stderr,
			"Received a not expected application data type (%d != %d)"
			" message on SensedDataInterestReceiver!\n",
			nrAppDataType->getVal(),
			pubData_->appDataType);
		abort();
	}
*/
	switch (nrClass->getVal())
	{
		case NRAttribute::INTEREST_CLASS:
			pubData_->subscriptions++;
			node_->onSubscription(pubData_, data);
			break;

		case NRAttribute::DISINTEREST_CLASS:
			pubData_->subscriptions--;
			node_->onUnsubscription(pubData_, data);
			break;
	}
}

bool CommonNodeDiffApp::interested(NRAttrVec * data)
{
	NRAVList::iterator it;

	for (it = subscriptions_.begin(); it != subscriptions_.end(); it++)
	{
		NRAttrVec * attrs = *it;

		PrintAttrs(attrs);
		PrintAttrs(data);

		if (OneWayMatch(attrs, data))
			return true;
	}

	return false;
}
