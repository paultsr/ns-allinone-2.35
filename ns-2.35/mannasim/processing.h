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
/// Class that simulates the processing activity on sensed data. It 
/// constains a processed data buffer witch is accessed  by the sensing 
/// in orther to disseminate processing results. Processing is a 
/// base class that can be specialized to simulate different types of data 
/// processing algorithms.
/// 
/// authors: Linnyer B. Ruiz
///	         Fabricio A. Silva
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
#ifndef __processing_h__
#define __processing_h__

#include <common/packet.h>

#include "battery.h"
#include "sensorNode.h"
#include "sensedData.h"
#include "onDemandData.h"
#include "dataGenerator.h"
#include "onDemandParameter.h"

/// Processor instructions count for aggregation and event check (greater than,
/// equal...) operations.
#define AGGREGATION_INSTRUCTIONS 200
#define EVENT_CHECK_INSTRUCTIONS 200

class SensorBaseApp;

/// Simulates the sensor data processing activity.
///
/// extends TclObject
class Processing : public TclObject
{
	public:
	
		/// Constructor.
		Processing();
		
		/// ??? This method MUST be overloaded.
		virtual void recvData(AppData* data_) = 0;
	
		/// NS-2 Function.
		int command(int argc, const char*const* argv);
        
        /// Simulates sensed data processing. This method
        /// MUST be oveloaded.
        virtual void processSensedData(AppData* data_) = 0;
        
        /// Simulates sensed data processing. Deals wih event occurence 
        /// verification for EVEN_DRIVEN applications. Don't need to be 
        /// overloaded.
		AppData* processSensedData(AppData* data_, AppData* eventData_);

		/// Methods for methods for request messages treatment.
		virtual AppData * processRequest(AppData* data);
		virtual AppData * process_real_request(AppData* data_, int operation);
		virtual AppData * process_buffer_request(AppData* data_, int operation);
		virtual AppData * process_request_data(OnDemandParameter* parameter, int request_type);

		/// NOT IMPLEMENTED YET.
		virtual AppData * process_average_request(AppData* data_, int operation);
		
		/// Accessor methods
		virtual SensedData* getProcessedData();
		virtual void resetData();
		virtual void setApp(SensorBaseApp* app);		
		
	protected:
	
		/// Processed data buffer.
		SensedData* info_;
		
		/// Application attached to the wireless sensor application. Used 
		/// in on demand processing. 
		SensorBaseApp * app_;
		
		/// Sensor node where the processing takes place. Used for energy
		/// contability propose.
		SensorNode* sensor_node_;
};

#endif
