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
/// Implementation of the classes used by all common sensor nodes of a Wireless
/// Sensor Network. These classes represents the sensing dynamics: sensing, 
/// processing, and disseminating.
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#include "sensorBaseApp.h"


/// Whenever the disseminating timer expires, the application disseminateData()
/// method should be invoked. The timer should also be rescheduled.
void DisseminatingTimer::expire(Event*)
{
	double dissT;
	app_->disseminateData();
	
	dissT = app_->getExpireTime();
	if (dissT != -1)
	{
		resched(dissT);
	}
}


/// Default constructor. Binds TCL script parameters to C++ code variables and
/// creates a disseminating timer.
SensorBaseApp::SensorBaseApp():Application()
{
	bind("disseminating_type_",&disseminating_type_);
	bind("disseminating_interval_",&disseminating_interval_);
	bind("destination_id_",&destination_id_);
	
	dissTimer_ = new DisseminatingTimer(this);
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int SensorBaseApp::command(int argc, const char*const* argv)
{
	if(argc == 2)
	{		
		if(strcmp(argv[1],"start") == 0)
		{
			/// Initiates the sensing, processing and disseminating activity
			SensorBaseApp::start();	
			return TCL_OK;
		}
		if(strcmp(argv[1],"stop") == 0)
		{
			/// Stops the sensing, processing and disseminating activity
			SensorBaseApp::stop();
			return TCL_OK;
		}
	}
	if(argc == 3)
	{
		if(strcmp("node", argv[1]) == 0)
		{
			/// Attach a node to the application
			sensor_node_ = (SensorNode*)TclObject::lookup(argv[2]);
			if (sensor_node_)
			{
				return TCL_OK;
			}
			fprintf(stderr,"Error: %s object not found",argv[2]);
			return TCL_ERROR;
		}
		if (strcmp("attach-processing",argv[1]) == 0)
		{
			/// Attach a processing activity to the application
			processing_ = (Processing*)TclObject::lookup(argv[2]);
			processing_->setApp(this);
			if (processing_)
			{
				return TCL_OK;
			}
			fprintf(stderr,"Error: %s object not found",argv[2]);
			return TCL_ERROR;
		}
		if(strcmp(argv[1],"attach_data_generator") == 0)
		{
			/// Attach a data generator to the application
			DataGenerator* gen = (DataGenerator*) TclObject::lookup(argv[2]);
			if(!gen)
			{
				return TCL_ERROR;
			}
			gen->insertNewApp(this);
			insertNewGenerator(gen);
			return TCL_OK;
		}
	}
	return Application::command(argc,argv);
}

/// Returns the expire time for diferente types of data dissemination.
double SensorBaseApp::getExpireTime(){

	switch(disseminating_type_)
	{
		case PROGRAMED:
			return disseminating_interval_;
		case CONTINUOUS:
			return -1;
		case ON_DEMAND:
			return -1;
		case EVENT_DRIVEN:
			return -1;
		default: 
			fprintf(stderr, "Unrecognized type of dissemination!\n");
			abort();
			break;
	}
}

/// Shedule the first processing/disseminating event if these activity type 
/// isn�t the continuous one.
void SensorBaseApp::start()
{	
	double dissT;
	dissT = getExpireTime();
	
	if (dissT != -1)
	{
		dissTimer_->resched(dissT);
	}

	DataGenList::iterator it;
	DataGenerator* gen;
	for(it = gen_.begin(); it != gen_.end(); it++)
	{	
		gen = *it;
		gen->start();
	}
}

/// Drops all the scheduled events.
void SensorBaseApp::stop()
{	
	if (dissTimer_->status() == TIMER_PENDING)
	{
		dissTimer_->cancel();
	}

	/// Stops all DataGenerators that are attached to this application.
	DataGenList::iterator it;
	DataGenerator* gen;
	for(it = gen_.begin(); it != gen_.end(); it++)
	{
		gen = *it;
		gen->stop();
	}
}

/// Inserts a new DataGenerator to application DataGenerator list.
void SensorBaseApp::insertNewGenerator(DataGenerator* gen)
{
	gen_.push_back(gen);
}

/// Returns the DataGenerator list for the application.
DataGenList SensorBaseApp::getGenList()
{
	return gen_;
}

/// Return the sensor node attached to the application.
SensorNode * SensorBaseApp::sensor_node()
{ 
	return sensor_node_;
}
