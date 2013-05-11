///  
/// Copyright (C) 2003-2005 Federal University of Minas Gerais
/// 
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
/// 
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
/// MA 02110-1301, USA.
/// 
///
/// Base class for the other sensor nodes applications. It is an 
/// abstract class that should be implemented in different ways 
/// by specialized applications. It is derived from Application NS class
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#ifndef __sensing_app_h__
#define __sensing_app_h__

using namespace std;

#include <list>
#include <apps/app.h>
#include <common/agent.h>
#include <common/ns-process.h>
#include <common/packet.h>
#include <tools/rng.h>

#include "battery.h"
#include "processing.h"
#include "sensorNode.h"
#include "onDemandData.h"
#include "dataGenerator.h"
#include "onDemandParameter.h"


/// Disseminate and sensing type enumeration
enum
{
	PROGRAMED 	 = 0,
	CONTINUOUS 	 = 1,
	ON_DEMAND 	 = 2,
	EVENT_DRIVEN = 3
};

#define TRAP_ 0
#define MSG_BYTES_ 32
#define INSTRUCTIONS_ 200
#define SENSING_TIME_ 0.01
#define DEFAULT_PORT_ 2020

#define SENSOR_REPORT_ 0
#define LOCALIZATION_ 1
#define INITIAL_GROUP_ 2
#define SOLICITATION_ 3
//#define REQUEST_ 4
#define DESTINATION_ 5


class SensedData;
class Processing;
class SensorBaseApp;
class DataGenerator;

/// Type definition for a DataGeneration STL list.
typedef list<DataGenerator*> DataGenList;

/// Disseminating timer for collect data.
class DisseminatingTimer : public TimerHandler {
	
	public:
		DisseminatingTimer(SensorBaseApp* app) : TimerHandler(), app_(app) {}
		inline virtual void expire(Event*);

	protected:
		
		/// SensorBaseApp used to disseminate the data
		SensorBaseApp* app_;
};


/// Base class for common-nodes, cluster-heads, managed nodes. Should be 
/// specialized for each specific node application. 
/// extends NS-2 Application class
class SensorBaseApp : public Application{
	
	public:
		/// standard constructor
		SensorBaseApp();

		/// NS-2 function
		virtual int command(int argc, const char*const* argv);
		
		/// Control functions for simulation applications.
		void start();
		void stop();

		/// Accessor methods.
		DataGenList getGenList();
		SensorNode * sensor_node();

		friend class DisseminatingTimer;
		friend class DataGenerator;
		friend class Processing;

	protected:	
		/// Array of Application Data Generators. It should be especified in the 
		/// moment of the application creation.
		DataGenList gen_;

		/// Instance of the object that processes sensed data.
		Processing* processing_;

		/// Refers to the sensor nodes that contains this application
		SensorNode* sensor_node_;

		/// Indicates the way the node disseminate sensed data.
		/// See disseminating enumeration above for disseminating valid values.
		int disseminating_type_;

		/// Indicates the nodes disseminating interval.
       /**
		 * Used to determine the dissemination interval. If this interval is 
		 * zero and the dissemination type is one, then the interval should be
		 * obtained from a normal distribution. If the dissemination type
		 * is 2, the dissemination will be determined by the received query 
		 * or event.
		 */
		double disseminating_interval_;

		/// Destination node for the sensed data. 
		int destination_id_;

		/// Dissemination timer, used for CONTINOUS and PROGRAMED dissemination.
		DisseminatingTimer* dissTimer_;

		virtual void insertNewGenerator(DataGenerator* gen);		
		double getExpireTime();
		
		/// IMPORTANT: the next four methods should be overwitten by specialized
		/// classes.
		
		/// Sends collected data (after processing activity) to the destination 
		/// node.
		virtual void disseminateData(){}

		/// Sends collected data (after processing activity) to the destination 
		/// node. Collected data is passed as a parameter.
		virtual void disseminateData(AppData* data){}
	
		/// Receives sensed data and performs some kind of processing. These 
		/// function should be overloaded.
		virtual void recvSensedData(AppData* data_){};
		virtual void recvSensedData(AppData* data_, AppData* eventData_){};
};

#endif
