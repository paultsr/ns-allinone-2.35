/* MannaNMP.cc
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * Implementation of MannaNMPAgent, a class for
 * MannaNMP protocol using directed diffusion.
 */
#include "../management/managementData.h"
#include "mannanmp.h"
#include <assert.h>
#include <dr.hh>

using namespace mib;

/* Constructs the agent */
MannaNMPAgent::MannaNMPAgent(MIBTable * mib, NR * dr)
{
	assert(mib && dr);

	this->mib_ = mib;
	this->dr_  = dr;
}

/* Receive data from directed diffusion */
void MannaNMPAgent::recv(NRAttrVec * data, handle h)
{
	NRSimpleAttribute<int> * nrClass;

	// Get message's class and validate it
	nrClass = NRClassAttr.find(data);

	if (!nrClass)
	{
		fprintf(stderr, "Received a bad diffusion packet!\n");
		abort();
	}

	switch (nrClass->getVal())
	{
		case NRAttribute::INTEREST_CLASS:
			onSubscription(data, h);
			break;

		case NRAttribute::DISINTEREST_CLASS:
			// Nothing here until this moment
			break;
	}
}

/* Occurs when received a subscription */
void MannaNMPAgent::onSubscription(NRAttrVec * data, handle h)
{
	NRSimpleAttribute<ManagementData *> * messageAttr;
	ManagementData *                      message;

	// Get management data
	messageAttr = ManagementRequestAttr.find(data);

	assert(messageAttr != NULL);

	if (messageAttr == NULL)
	{
		fprintf(stderr, "Como assim 1?\n");
		abort();
	}

	message = messageAttr->getVal();

	assert(message != NULL);

	if (message == NULL)
	{
		fprintf(stderr, "Como assim 2?\n");
		abort();
	}

	switch (message->getOperation())
	{
		case GET:
			printf("MANNANMP: GET\n");
			onGetMessage((GetData *) message, h);
			break;

		case SET:
			printf("MANNANMP: SET\n");
			onSetMessage((SetData *) message, h);
			break;

		case RESPONSE:
		case TRAP:
			fprintf(stderr,
				"Agent shouldn't receive a RESPONSE or TRAP"
				" message!\n");
			abort();

		default:
			fprintf(stderr,
				"Unknown management operation %d!\n",
				message->getOperation());
			abort();
	}
}

/* onGetMessage(message, h)
 *
 * Occurs when received a get message.
 */
void MannaNMPAgent::onGetMessage(GetData * message, handle h)
{
	NRAttrVec             attrs;          // Attributes of diffusion message
	KeyValueData          data(RESPONSE); // Response data
	GetData::MIBKeyList * requestList;
	ManagementData *      dataCopy;

	requestList = message->getRequestList();

	printf("Sending response for key(s)");
	
	for (GetData::MIBKeyList::iterator it = requestList->begin();
		it != requestList->end();
		it++)
	{
		mibKey         key = *it;
		MIBEntryBase * entry = mib_->getEntry(key)->copy();

		data.addEntry(entry);

		// DEBUG!!!
		printf(" %d = ", key);

		switch (entry->getType())
		{
			case INT32:
				printf("%d (INT32)", ((MIBEntry<int> *) entry)->getValue());
				break;

			case LONG:
				printf("%ld (LONG)", ((MIBEntry<long> *) entry)->getValue());
				break;

			case FLOAT:
				printf("%f (FLOAT)", ((MIBEntry<float> *) entry)->getValue());
				break;

			case DOUBLE:
				printf("%lf (DOUBLE)", ((MIBEntry<double> *) entry)->getValue());
				break;
		}
	}

	printf(".\n");

	dataCopy = (ManagementData *) data.copy();

	attrs.push_back(
		ManagementResponseAttr.make(
			NRAttribute::IS, dataCopy));

	dr_->send(hnd_, &attrs);

	ClearAttrs(&attrs);
}

/* Occurs when received a set message */
void MannaNMPAgent::onSetMessage(SetData * message, handle h)
{
	SetData::MIBEntryList * list;
	char                    buf[32];

	list = message->getEntries();

	for (SetData::MIBEntryList::iterator it = list->begin(); it != list->end(); it++)
	{
		mibKey         key;
		MIBEntryBase * entry;

		key   = (*it)->getKey();
		entry = mib_->getEntry(key);

		(*it)->toString(buf);

		printf("     Setting %d = %s\n", key, buf);

		entry->setValue(buf);
	}
}

