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
/// This class simulates the behavior of a cluster head in a wireless
/// sensor network. Dispates its more capacity to process and disseminate
/// sensed data, a cluster head must also have a group of nodes under
/// its responsability - a child list.
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
#include "clusterHeadApp.h"

/// This static class Provides an instance of the ClusterHeadApp class in 
/// the TCL simulation script.
static class ClusterHeadAppClass : public TclClass{

	public:
		ClusterHeadAppClass() :	TclClass("Application/SensorBaseApp/ClusterHeadApp"){}
		TclObject* create(int, const char*const*){
				return (new ClusterHeadApp);
		}

}class_cluster_head_app;

/// Default constructor. Does notjhing, only calls the super class
/// (SensorBaseApp) default constructor.
ClusterHeadApp::ClusterHeadApp() : SensorBaseApp()
{
}

/// ClusterHeadApp object needs this method to receive request (on demand) 
/// messages. 
/// NOTE: Specialization of process_data from the NS-2 Process::process_data 
/// function. This method is invoked in Agent::recv().
void ClusterHeadApp::process_data(int size, AppData* data){
		
	if(data == NULL)
	{
		fprintf(stderr,"ClusterHeadApp::process_data() --> data is NULL\n");
		abort();
	} 
	
	/// The received data isn�t for "me"			
	if(((SensedData*)data)->node_id() != destination_id_)
	{
		/// It is a broadcast message from other Cluster Head
		if(data->type() == ON_DEMAND_DATA)
		{
			return;
		}
		
		/// The node from where the message came isn�t on the
		/// Cluster Head child list, insert node on child list.
		if(!search_child(((SensedData*)data)->node_id()))
		{
			insert_child(((SensedData*)data)->node_id());
		}

		if(processing_ == NULL)
		{
			fprintf(stderr,"ClusterHeadApp::process_data() --> processing is NULL\n");
			abort();
		}	
		
		/// Here occurs the data processing
		processing_->recvData(data);
		
		/// If the disseminating type is continuous, send data 
		/// immediately
		if(disseminating_type_ == CONTINUOUS)
		{
			disseminateData();
		}
		
	}
	else
	{
		/// The received data is for "me" and I am an Access Point
		//is the AP
		//implementar para recebimento de request
		if(data->type() == ON_DEMAND_DATA)
		{
			//ver como passar este m�todo para o processing, j� que ser� 
			//diferente do nodo comum
			processRequest(data);
		}
	}
	printf("Cluster Head Node %d - Received message - Time %.3f - Source Node %d!\n",
			sensor_node_->nodeid(), Scheduler::instance().clock(),((SensedData*)data)->node_id());
}

/// Disseminate the sensed data throught the network. 
void ClusterHeadApp::disseminateData()
{
	if(processing_ == NULL)
	{
		fprintf(stderr,"ClusterHeadApp::disseminateData() --> processing is NULL\n");
		abort();
	}

	/// Get the processed data.
	SensedData* data_ = processing_->getProcessedData();

	/// If there is any information to be disseminated, do it!
	if(data_ != NULL)
	{
		printf("Cluster Head Node %d - Disseminating Data - Destination Node %d - Time %.3f\n",
						sensor_node_->nodeid(), destination_id_,Scheduler::instance().clock());

		/// Configure agent to disseminate the parameter data.
		agent_->daddr() = destination_id_;
		agent_->dport() = DEFAULT_PORT_;

		/// verificar como � a mensagem sem gerenciamento, pois TRAP � de 
		/// gerenciamento.
		data_->msgType() = TRAP_;
		data_->eventType() = SENSOR_REPORT_;
		data_->node_id() = sensor_node_->nodeid();

		/// Send the message.
		agent_->sendmsg(CH_MSG_BYTES_,data_->copy());
		processing_->resetData();
	}
}

/// Forwards a request message for all child in child list.
void ClusterHeadApp::processRequest(AppData* data)
{
	if(data == NULL)
	{
		fprintf(stderr,"ClusterHeadApp::processRequest() --> data is NULL\n");
		abort();
	}	

	printf("Cluster Head Node %d sending request data!\n",sensor_node_->nodeid());	

	/// Configure agent to disseminate the parameter data.
	agent_->daddr() = IP_BROADCAST;
	agent_->dport() = DEFAULT_PORT_;
	
	/// Changes the address of source node so the child will receive the 
	/// message.
	((SensedData*)data)->node_id() = sensor_node_->nodeid();

	/// Send the message.
	agent_->sendmsg(CH_MSG_BYTES_,data->copy());
}

/// Inserts a nwe item in Cluster Head child list
void ClusterHeadApp::insert_child(int id)
{
	child_list.push_back(id);
}

/// Removes all itens that are equal to id. 
void ClusterHeadApp::remove_child(int id)
{
  child_list.remove(id);
}

/// Returns a boolean indicating if node "id" is in cluster head
/// child list.
bool ClusterHeadApp::search_child(int id)
{
	for(IdList::iterator it = child_list.begin(); it != child_list.end(); ++it)
	{
		if(*it == id)
		{
			return true;
		}
	}
	return false;
}


