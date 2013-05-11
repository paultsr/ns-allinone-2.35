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
#include "processing.h"

/// Default constructor. Only call superclass constructor.
Processing::Processing() : TclObject()
{
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int Processing::command(int argc, const char*const* argv)
{
	if (argc == 3)
	{
		if (strcmp("node",argv[1]) == 0)
		{
			sensor_node_ = (SensorNode*)TclObject::lookup(argv[2]);
			if (sensor_node_)
				return TCL_OK;
			fprintf(stderr,"Error: %s object not found",argv[2]);
			return TCL_ERROR;
		}
	}
    return TclObject::command(argc, argv);
}

/// Receives common nodes sensed data from a EVENT_DRIVEN wireless sensor
/// application. Verifies whether an event (for example, temperature sensed
/// greater than X) occured or not. If so, process it and give it to 
/// dissemination. Otherwise, drop the information.
AppData* Processing::processSensedData(AppData* data_, AppData* eventData_)
{
   
    /// This activity wastes sensor node energy! PROCESSING WASTE HERE!
	((Battery*)sensor_node_->energy_model())->DecrProcessingEnergy(
									EVENT_CHECK_INSTRUCTIONS,
									sensor_node_->instructionsPerSecond(),
									sensor_node_->processingPower());

	if(eventData_->checkEvent(data_))
	{
		printf("Processing::processSensedData - Event detected!\n");
       	return data_;
	}
	else
	{
		return NULL;
	}
}

/// Method that deals with request messagens. First the request type is
/// retrieved from data, then for each OnDemandParameter associated
/// with the request message a specific process, based on query request,
/// is realized.
AppData * Processing::processRequest(AppData* data)
{
	
	/// Data received isn't OnDemandData.
	if(data->type() != ON_DEMAND_DATA)
	{
		fprintf(stderr,"The data received is not a onDemandData.");
		return NULL;
	}
	
	OnDemandData* onDemandData = (OnDemandData*)data;
	int request_type = onDemandData->requestType();
	AppDataList list = onDemandData->getData();
	
	AppDataList::iterator it;
	OnDemandParameter* parameter;
  
	for (it = list.begin(); it != list.end(); it++)
	{
		if(((AppData*)(*it))->type() != ON_DEMAND_PARAMETER)
		{
			fprintf(stderr,"The data received is not a onDemandParameter.");
			return NULL;
		}
		
		/// Gets OnDemandParameter associated with request an process it.
		parameter = (OnDemandParameter*)(*it);
		return (process_request_data(parameter, request_type));
	}
}

/// Manages diferent requests from an outsider observer. The request_type 
/// parameter is used to select the further data processing
AppData* Processing::process_request_data(OnDemandParameter* parameter, int request_type)
{
	int operation = parameter->operation();
	AppData* data_ = parameter->data();

	/// Choose the correct method given the request type.
	switch (request_type)
	{
		case REAL:
			return process_real_request(data_, operation);
			break;
		
		case BUFFER:
		    return process_buffer_request(data_, operation);
		    break;
		    
    	case AVERAGE:
    		return process_average_request(data_, operation);
    		break;
    	
    	default:
    		fprintf(stderr,"Processing::process_request_data - Invalid Request Type.");
			break;
	}
	return NULL;	
}

/// Process request messages of REAL type. In this kind of data request,
/// the sensor node drops all data from its buffer, collect new one and 
/// process it.
AppData* Processing::process_real_request(AppData* data_, int operation)
{
	/// New synthetic data needs to be generated. The applications list of 
	/// data generator is used.
	DataGenList list = app_->getGenList();
	DataGenList::iterator it;
	
	DataGenerator* gen;
	AppData* newData;
	
	/// Processed data.
	SensedData* responseData = new SensedData();

  	/// For each data generator associated with the application app_...
  	for(it = list.begin(); it != list.end(); it++)
  	{
  		gen = (*it);
  		
  		/// ... checks if generated data type is the same of the sample data...
  		if(gen->type() == data_->type())
  		{
  			/// ... generates new data and process it.
  			newData = gen->collect();
  			if(data_->compareData(newData, operation))
  			{
  				responseData->insertNewData(newData);
  				return responseData;
  			}
  		}
  	}
	return NULL;
}

/// Process request messages of BUFFER type. In this kind of data request,
/// data storage in sensor data buffer is processed and if satisfys the 
/// request message it is given to dissemination.
AppData* Processing::process_buffer_request(AppData* data_, int operation)
{
	AppDataList list;
	
	if(info_ != NULL)
	{
		list = info_->getData();
	}
	else
	{
		printf("Processing::process_buffer_request - Buffer is empty.\n");
		return NULL;
	}
 
	AppData* bufferData_;
	AppDataList::iterator it;
 	SensedData* responseData_ = new SensedData();
  
  	//each AppData (temperature, ...) should have this method
  	for(it = list.begin(); it != list.end(); it++)
  	{
  		bufferData_ = (*it);
  		
  		/// ... checks if buffer data type is the same of the sample data...
  		if(bufferData_->type() == data_->type())
  		{
  			if(data_->compareData(bufferData_, operation))
  			{
	  			/// ... buffer data attends request requiriments, add it to
	  			/// response data.
				responseData_->insertNewData(bufferData_);
      		}
    	}
  	}
  
  	/// There were data in buffer, disseminate it.
  	if(responseData_->existsData())
  	{
  		return responseData_;
  	}
	return NULL;
}

/// Process request messages of AVERAGE type. In this kind of request messages
/// an average of the data storage in sensor node buffer is computed and 
/// returned to the dissemination.
///
/// NOT IMPLEMENTED YET.
AppData * Processing::process_average_request(AppData* data_,int operation)
{ 
}

/// Reset the processed data making it NULL.
void Processing::resetData()
{
	info_ = NULL;
}

/// Returns data generated by the processing activity.
SensedData * Processing::getProcessedData()
{
	return info_;
}

/// Set application attached to the wireless sensor application.
void Processing::setApp(SensorBaseApp* app)
{
	app_ = app;
}
