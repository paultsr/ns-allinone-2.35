///  
/// Copyright (C) 2003-2005 Federal University of Minas Gerais
/// 
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
/// MA 02110-1301, USA.
/// 
/// Implementation of the application that makes the comunication between inside 
/// the WSN and outside it: the access point (AP). The AP is called "Sink" when 
/// we are dealing with a flat WSN, and "Base Station" when it comes to a 
/// hierachical WSN.
///
/// authors: Linnyer B. Ruiz
///	         Fabr�cio A. Silva
///			 Thais Regina de M. Braga 
///  		 Kalina Ramos Porto
///
/// code revisor: Carlos Eduardo R. Lopes
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
///
/// This project was financially supported by The National Council 
/// for Scientific and Technological Development (CNPq) from the 
/// brazilian government under the process number 55.2111/2002-3.
///
#include "accessPointApp.h"

/// This static class Provides an instance of the AccessPoint class in 
/// the TCL simulation script.
static class AccessPointClass : public TclClass
{
	public:
		AccessPointClass() : TclClass("Application/AccessPointApp"){}
		TclObject* create(int, const char*const*){
			return (new AccessPointApp);
		}
}class_access_point_app;

/// AccessPoint constructor. Binds TCL script parameters to C++ code variables.
AccessPointApp::AccessPointApp():Application()
{
	bind("destination_id_",&destination_id);
	bind("request_type_",&request_type);
	bind("node_id_",&node_id);
	
	//diretorio de saida de acordo com cenario simulado
	bind("diretorio",&diretorio);
	
	/// Default value for request type (REAL)
	request_type = 0;
	onDemandData_ = NULL;
} 

/// 
/// NOTE: Specialization of process_data from the NS-2 Process::process_data 
/// function. This method is invoked in Agent::recv().
void AccessPointApp::process_data(int size, AppData* data)
{
  /// Modified by Fabricio Silva. Prevents broadcast messages 
  /// to be erroneous received.
  if(data->type() == ON_DEMAND_DATA)
  {
    return;
  }



  AppDataList list = ((SensedData*)data)->getData();
  printf("Access point - Received a message with %d elements\n",
         (int)list.size());  

  for(AppDataList::iterator it = list.begin(); it != list.end(); it++)
  {
    printf("Message received from node %d with %lf delay ::: %lf %lf\n", 
    	((SensedData *) data)->node_id(),
    	Scheduler::instance().clock() - ((SensedData *) data)->timeStamp(),
    	((SensedData *) data)->timeStamp(), 
    	Scheduler::instance().clock());
  }
//  forward_data(size, data);
}

/// 
void AccessPointApp::forward_data(int size, AppData* data){

  AppDataList list = ((SensedData*)data)->getData();
  //AppDataList::iterator it;
  //for(it = list.begin(); it != list.end(); it++){
  //  TemperatureAppData* d = (TemperatureAppData*)(*it);
  //  //printf("AccessPointApp::forward_data: %f\n",d->data());
  //}

  agent_->daddr() = destination_id;
  agent_->dport() = DEFAULT_PORT_;
  agent_->sendmsg(AP_MSG_BYTES_, data->copy());

}

/// Send on demand message.
void AccessPointApp::send_request()
{
	printf("<<Access Point sending a request message.>>\n");
	if(onDemandData_ == NULL)
	{
		fprintf(stderr,"AccessPointApp::send_request - no parameter defined yet.");
		return;
	}
	
	//temporario este TRAP e SENSOR_REPORT
	onDemandData_->msgType() = TRAP_;
	onDemandData_->eventType() = SENSOR_REPORT_;
	onDemandData_->node_id() = node_id;
	
	agent_->daddr() = IP_BROADCAST;
	agent_->dport() = DEFAULT_PORT_;
	agent_->sendmsg(AP_MSG_BYTES_,onDemandData_->copy());
	onDemandData_ = NULL;
}

int AccessPointApp::command(int argc, const char*const* argv)
{
	if(argc == 2)
	{
		if(strcmp("send_request",argv[1]) == 0)
		{
			send_request();
			return TCL_OK;
		}
		else
		{
			if(strcmp("stop",argv[1]) == 0)
			{
				stop();
				return TCL_OK;
			}
		}
	}
      

  /**
   * Syntax form:
   * 
   * access_point "add_temp_data_param 25.0 0" OU
   *
   * access_point "add_temp_data_param 25.0" --> uses the default (0)
   *
   * Creates a OnDemandParameter of temperature that are greater (0) than 25.0
   *
   * For each SensedData (temperature,...)
   *
   */
	if(argc == 3)
	{
		if(strcmp("add_temp_data_param",argv[1]) == 0)
		{
			create_parameter(new  TemperatureAppData(atof(argv[2]),
                                                    (double)(Scheduler::instance().clock())),
                             0);
			return TCL_OK;
		}
		else
		{
			if(strcmp("add_carbon_data_param",argv[1]) == 0)
			{
				create_parameter(new CarbonMonoxideAppData(atof(argv[2]),
                                                           (double)(Scheduler::instance().clock())),
                                 0);
				return TCL_OK;
			}
		}
	}
	
	if(argc == 4)
	{
		if(strcmp("add_temp_data_param",argv[1]) == 0)
		{
			create_parameter(new TemperatureAppData(atof(argv[2]),
                                                   (double)(Scheduler::instance().clock())), 
                             atoi(argv[3]));
			return TCL_OK;
		}
		else
		{
			if(strcmp("add_carbon_data_param",argv[1]) == 0)
			{
				create_parameter(new CarbonMonoxideAppData(atof(argv[2]),
                                                          (double)(Scheduler::instance().clock())), 
                                 atoi(argv[3]));
				return TCL_OK;
			}
		}
	}
	return Application::command(argc,argv);
}

/// 
void AccessPointApp::create_parameter(AppData* data, int operation)
{
	if(onDemandData_ == NULL)
	{
		onDemandData_ = new OnDemandData(request_type);
	}
	onDemandData_->insertNewData(new OnDemandParameter(data, operation));
}

void AccessPointApp::stop()
{
  /*char diretorio_saida[60];

  switch(diretorio)
  {
    case 0:
      sprintf(diretorio_saida,"data/grid");
      break;
    case 1:
      sprintf(diretorio_saida, "data/aleatorio");
      break;
    case 2:
      sprintf(diretorio_saida, "data/cluster_grid");
      break;
    default:
      printf("erro no diretorio\n");
      exit(0);
  }

  Contabiliza::instance()->imprime(diretorio_saida);*/
}
