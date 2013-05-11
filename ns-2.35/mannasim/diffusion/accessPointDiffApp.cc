/* accessPointDiffApp.cc
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * Implementation of class AccessPointDiffApp.
 *
 * This class is an application for access point, using
 * directed diffusion.
 */
#include "accessPointDiffApp.h"

static class AccessPointDiffAppClass : public TclClass
{
	public:
		AccessPointDiffAppClass() : TclClass("Application/AccessPointDiffApp") {}

	TclObject * create(int, const char * const *)
	{
		return new AccessPointDiffApp();
	}
} access_point_diff_app;


// Constructor
AccessPointDiffApp::AccessPointDiffApp()
{
//	callback_ = new MessageReceiver(this);
}

// Tcl command interpreter
int AccessPointDiffApp::command(int argc, const char * const * argv)
{
	if (argc > 2)
	{
		if (!strcmp(argv[1], "subscribe"))
			return execTclSubscribe(argc - 2, argv + 2);
	}

	return DiffApp::command(argc, argv);
}

// Receive a packet from diffusion protocol
void AccessPointDiffApp::recv(NRAttrVec * data, NR::handle my_handle)
{
	NRSimpleAttribute<int> * nrClass;

	nrClass = NRClassAttr.find(data);

	if (!nrClass)
	{
		fprintf(stderr, "Received a bad diffusion packet!");
		abort();
	}

	switch (nrClass->getVal())
	{
		case NRAttribute::INTEREST_CLASS:
			printf("Access point received an Interest message!\n");
			break;

		case NRAttribute::DISINTEREST_CLASS:
			printf("Access point received a Disinterest message!\n");
			break;

		case NRAttribute::DATA_CLASS:
			printf("*** Access point received a Data message!\n");
			recvData(data, my_handle);
			break;
	}
}

// Receive a data message
void AccessPointDiffApp::recvData(NRAttrVec * data, NR::handle my_handle)
{
	NRSimpleAttribute<AppDataType> * nrAppDataType;

	nrAppDataType = AppDataTypeAttr.find(data);

	if (!nrAppDataType)
	{
		fprintf(stderr, "Bad data message!");
		abort();
	}

	printf("Data message contains application data type %d.\n",
		nrAppDataType->getVal());

	switch (nrAppDataType->getVal())
	{
		case SENSED_DATA:
			recvSensedData(data, my_handle);
			break;

		default:
			recvSpecificSensedData(nrAppDataType->getVal(), data, my_handle);
			break;
	}
}

void AccessPointDiffApp::recvSensedData(NRAttrVec * data, handle h)
{
	NRSimpleAttribute<AppData *>   * nrSensedData;
	SensedData *                     sensedData;

	/* Look for sensed data */
	nrSensedData = SensedDataAttr.find(data);

	if (nrSensedData)
	{
		AppDataList list;

		sensedData = (SensedData *) (nrSensedData->getVal());
		list       = sensedData->getData();
	
		printf("<<Received %d elements>>", list.size());

		for (AppDataList::iterator it = list.begin();
			it != list.end();
			it++)
		{
			switch ((*it)->type())
			{
				case TEMPERATURE_SENSED_DATA:
					printf("  Temperature: %lf on %lf\n",
						((TemperatureAppData *) *it)->data(),
						((TemperatureAppData *) *it)->time());
					break;

				default:
					printf("  Unknown data type %d!\n",
						(*it)->type());
					printf("  FORCANDO Temperature: %lf on %lf\n",
						((TemperatureAppData *) *it)->data(),
						((TemperatureAppData *) *it)->time());
					break;
			}
		}
	}
	else
    {
		printf("AccessPointDiffApp::recvSensedData - There are no sensed data!\n");
    }
}

void AccessPointDiffApp::recvSpecificSensedData(AppDataType type, NRAttrVec * data, handle h)
{
	NRSimpleAttribute<double> * nrValue;
	NRSimpleAttribute<float>  * nrTimeStamp;
	NRSimpleAttribute<int>    * nrNodeID;

	nrValue     = SensedValueAttr.find(data);
	nrTimeStamp = TimeStampAttr.find(data);
	nrNodeID    = NodeIDAttr.find(data);

	printf("@ Received from node %d with %lf delay ::: %lf %lf\n", nrNodeID->getVal(), Scheduler::instance().clock() - nrTimeStamp->getVal(), nrTimeStamp->getVal(), Scheduler::instance().clock());

	switch (type)
	{
		case TEMPERATURE_SENSED_DATA:
			printf("<< Temperature: %lf >>\n", nrValue->getVal());
			break;

		default:
			printf("<< UNKNOWN TYPE: %lf >>\n", nrValue->getVal());
			break;
	}
}

// Subscribe to an interest
handle AccessPointDiffApp::subscribe(AppDataType appDataType, NRAttribute::operators op, double value)
{
	NRAttrVec attrs;
	handle    hnd;

	printf("Access point subscribed to application data type %d.\n", appDataType);

	attrs.push_back(
		NRClassAttr.make(NRAttribute::IS, NRAttribute::INTEREST_CLASS));

	attrs.push_back(AppDataTypeAttr.make(NRAttribute::EQ, appDataType));

	attrs.push_back(SensedValueAttr.make(op, value));

	hnd = dr_->subscribe(&attrs, this);

	ClearAttrs(&attrs);

	return hnd;
}

/*****************************************************************/
/* Tcl Commands                                                  */
/*****************************************************************/

int AccessPointDiffApp::execTclSubscribe(int argc, const char * const * argv)
{
	AppDataType             type;
	NRAttribute::operators  op;
	double                  value;

	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "SINTAXE: subscribe <type> <operator> [value]\n\n"
				"<operator> can be:\n"
				"    =     Equal\n"
				"    >     Greater than\n"
				"    <     Lesser than\n"
				"    !=    Not equal\n"
				"    >=    Greater or equal\n"
				"    <=    Lesser or equal\n"
				"    *     Any\n");
	}

	type = (AppDataType) atoi(argv[0]);

	if (argv[1][1] == 0)
	{
		switch (argv[1][0])
		{
			case '=':
				op = NRAttribute::EQ;
				break;
	
			case '>':
				op = NRAttribute::GT;
				break;

			case '<':
				op = NRAttribute::LT;
				break;

			case '*':
				op = NRAttribute::EQ_ANY;
				break;

			default:
				return TCL_ERROR;
		}
	}
	else if (argv[1][2] == 0)
	{
		if (argv[1][1] == '=')
		{
			switch (argv[1][0])
			{
				case '!':
					op = NRAttribute::NE;
					break;

				case '>':
					op = NRAttribute::GE;
					break;

				case '<':
					op = NRAttribute::LE;
					break;

				default:
					return TCL_ERROR;
			}
		}
		else
			return TCL_ERROR;
	}
	else
		return TCL_ERROR;

	if (argc == 3)
		value = atof(argv[2]);

	else if (op != NRAttribute::EQ_ANY)
		return TCL_ERROR;

	subscribe(type, op, value);

	return TCL_OK;
}
