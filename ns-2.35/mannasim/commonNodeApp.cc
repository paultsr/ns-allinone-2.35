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
/// Represents common-nodes application which performs data dissemination
/// using the disseminateData method, processing using processSensedData 
/// method and other functions using CommonNodeApp methods. 
/// The CommonNodeApp is a specialization of SensorBaseApp class.
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
#include "commonNodeApp.h"

/// This static class Provides an instance of the CommonNodeApp class in 
/// the TCL simulation script
static class CommonNodeAppClass : public TclClass
{
	public:
		CommonNodeAppClass() : TclClass("Application/SensorBaseApp/CommonNodeApp"){}
		TclObject* create(int, const char*const*){
			return (new CommonNodeApp);
		}

}class_common_node_app;

/// Default constructor. Does notjhing, only calls the super class
/// (SensorBaseApp) default constructor.
CommonNodeApp::CommonNodeApp():SensorBaseApp()
{
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int CommonNodeApp::command(int argc, const char*const* argv)
{
	if (argc == 2)
	{				
		if (strcmp(argv[1],"start") == 0) {
			start();
			return TCL_OK;
		}
		if (strcmp(argv[1],"stop") == 0) {
			stop();
			return TCL_OK;
		}
	}
	return SensorBaseApp::command(argc, argv);
}

/// CommonNodeApp object needs this method to receive request (on demand) 
/// messages. 
/// NOTE: Specialization of process_data from the NS-2 Process::process_data 
/// function. This method is invoked in Agent::recv().
void CommonNodeApp::process_data(int size, AppData* data)
{	
	if (isDead())
	{
		return;
	}

	if(data == NULL)
	{
		fprintf(stderr,"CommonNodeApp::process_data() --> data is NULL\n");
		abort();
	}	
	
	/// If the message is for "me", process it (throught disseminanteDaTa()).
	if(((SensedData*)data)->node_id() == destination_id_)
	{
		printf("Common Node %d receive a message.\n",sensor_node_->nodeid());
		disseminateData(((SensedData*)processing_->processRequest(data))); 
	}
	else
	{
//		fprintf(stderr,"Source is not correct!!\n");	
	}
}

/// Disseminate the sensed data throught the network. 
void CommonNodeApp::disseminateData()
{
	if (isDead())
	{
		return;
	}

	if(processing_ == NULL)
	{
		fprintf(stderr,"CommonNodeApp::disseminateData() --> processing is NULL\n");
		abort();
	}

	// If there is information to be disseminated, do it!
	SensedData* data_ = processing_->getProcessedData();
	if (data_ != NULL){
		disseminateData(data_);
	    processing_->resetData();
	}	
}

/// Disseminate the sensed data throught the network. The data to be sent is
/// passed by reference. This function is also invoked by the it�s no parametric
/// version - disseminateData().
void CommonNodeApp::disseminateData(SensedData* data_)
{  
	if(data_ != NULL && !isDead())
	{
		/// For user information only.
		printf("Common Node %d - Disseminating data -  Time %.3f - Destination node %d\n",
				sensor_node_->nodeid(),Scheduler::instance().clock(),destination_id_);

		/// Configure agent to disseminate the parameter data.
		agent_->daddr() = destination_id_;
		agent_->dport() = DEFAULT_PORT_;
		
		/// verificar como � a mensagem sem gerenciamento, pois TRAP � de 
		/// gerenciamento.
	   	data_->msgType() = TRAP_;
     	data_->eventType() = SENSOR_REPORT_;
	   	data_->node_id() = sensor_node_->nodeid();
	   	
	   	/// Send the message.
     	agent_->sendmsg(MSG_BYTES_,data_->copy());
	}
}

/// Receives data from the sensing activity and performs data 
/// processing according to the processing object that
/// is attached to the node (Processing::processSensedData method).
void CommonNodeApp::recvSensedData(AppData* data_)
{
	if (isDead())
	{
		return;
	}

	if(data_ == NULL){
		fprintf(stderr,"CommonNodeApp::recvSensedData() --> data_ is NULL\n");
		abort();
	}
	
	if(processing_ == NULL)
	{
		fprintf(stderr,"CommonNodeApp::recvSensedData() --> processing is NULL\n");
		abort();
	}

	/// Here occurs the data processing
	processing_->processSensedData(data_->copy());

	/// If the disseminating type is continuous, send data 
	/// immediately
	if(disseminating_type_ == CONTINUOUS){
		disseminateData();
	}
}

/// Receives data from the sensing activity and performs data 
/// processing according to the processing object that
/// is attached to the node (Processing::processSensedData method).
/// This function is oriented to a EVENT_DRIVEN sensor network. It
/// verify if the data is of interest, and disseminate it only if it is.
void CommonNodeApp::recvSensedData(AppData* data_, AppData* eventData_)
{
	if (isDead())
		return;

	if(processing_ == NULL)
	{
		fprintf(stderr,"CommonNodeApp::recvSensedData() --> processing is NULL\n");
		abort();
	}	

	AppData* processedData_;

	/// Here occurs the data processing
	processedData_ = processing_->processSensedData(data_->copy(), 
													eventData_->copy());

	/// Data is of interest so disseminate it.
	if(processedData_ != NULL)
	{
		SensedData* sensedData = new SensedData();
		sensedData->insertNewData(processedData_);
		disseminateData(sensedData);
	}
}

/// Returns a boolean indicating if the node is dead or not. Dead means that the
/// node ran out energy.
inline bool CommonNodeApp::isDead()
{
	return ((Battery *) sensor_node_->energy_model())->energy() <= 0;
}
