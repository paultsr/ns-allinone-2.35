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
#ifndef __access_point_h__
#define __access_point_h__

#include <common/agent.h>
#include <common/packet.h>
#include <common/ns-process.h>


#include "processing.h"
#include "sensedData.h"
#include "onDemandData.h"
#include "sensorBaseApp.h"
#include "util/accounts.h"
#include "onDemandParameter.h"
#include "temperatureAppData.h"
#include "carbonMonoxideAppData.h"

/// Access point size messages.
#define AP_MSG_BYTES_ 64

/// Access point application. Simulates the behavior of an access point.
class AccessPointApp : public Application{

	public:
	
		/// Default Constructor
		AccessPointApp();
	
		/// Method called from the transport protocol for the application to 
		/// receive a message. Method from Process (ns-process.cc) overcharged
		virtual void process_data(int size, AppData* data);

		/// NS-2 function.
		int command(int argc, const char*const* argv);
		
		/// 
		virtual void stop();

	protected:
	
		/// 
		virtual void forward_data(int size, AppData* data);	

	private:

		/// Access point address.
		int node_id;

		/// Message node destination
		int destination_id;
	
		/// Object that represents the response of a request.
		OnDemandData* onDemandData_;
	
		/// Indicates the request type in on demand message (OnDemandData)
		int request_type;
		
		/// Variable used to indicates the directory indicates simulation 
		/// results directory. Temporary, used on wgrs04.
		int diretorio;

		void send_request();

		void create_parameter(AppData* data, int operation);
};

#endif
