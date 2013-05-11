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
/// Base class for the generation of synthetic data. Specialized sensed data,
/// such as temperture, magnetic field, video, among others, should extends 
/// this class and add the necessary modifications. 
/// 
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#include "dataGenerator.h"


/// Whenever the sensing timer expires, the generator generateData()
/// method should be invoked. The timer should also be rescheduled.
void SensingTimer::expire(Event*)
{
	gen_->generateData();
}

/// Constructor. Binds TCL script parameters to C++ code variables and
/// creates a disseminating timer.
DataGenerator::DataGenerator(AppDataType type) : TclObject()
{
	sensTimer_ = new SensingTimer(this);
	type_ = type;
	
	bind("sensing_type_",&sensing_type_);
	bind("sensing_interval_",&sensing_interval_);

}

/// Invokes the collect() function of the sensing object, and gives the 
/// result of it as a parameter for the processData() processing Object 
/// function. This simulates the behavior of a sensor node, when it senses 
/// the environment and then gives the obtained data to its 
/// computational part so it can process it.
void DataGenerator::generateData()
{
	double sensT;
	SensorAppList::iterator it;	
	SensorBaseApp *app_instance_;
	
	for(it = app_.begin(); it != app_.end(); it++)
	{
		app_instance_ = (*it);
      
      	/// Display node id for debug proposes
		if((app_instance_->sensor_node_->nodeid()) > 9)
		{
			printf("Node %d - ", app_instance_->sensor_node_->nodeid());
		}
		else
		{
      		printf("Node 0%d - ", app_instance_->sensor_node_->nodeid());
		}
		
		/// Collect sensed data and pass it to the application to processing
		if(app_instance_->disseminating_type_ == EVENT_DRIVEN)
		{ 
			app_instance_->recvSensedData(collect(), getMaximumAllowedValue());
		}
		else
		{
			app_instance_->recvSensedData(collect());
		}

		// The activity above wastes sensor node energy! ENERGY WASTE HERE!
		((Battery*)app_instance_->sensor_node_->energy_model())->
			DecrSensingEnergy(SENSING_TIME_,app_instance_->sensor_node_->sensingPower());
	}

	sensT = getExpireTime();
	if(sensT != -1)
	{
		// Reschedule the event according to sensing activity type
		sensTimer_->resched(sensT);
	}
}

/// Inserts interference on sensed data. NOT IMPLEMENTED YET.
/// It is recommended to specialize the method if it is going to be implemented.
void DataGenerator::insertInterference()
{
	fprintf(stderr,"DataGenerator::insertInterference() not implemented yet\n");
	abort();
}

/// Returns timer expire value, taking into account the sensing activity type.
double DataGenerator::getExpireTime()
{
	switch(sensing_type_)
	{
		case PROGRAMED:
			/// If it is a programed sensing activity, use the sensing_interval_, 
			/// determined by the user, as the expire timer
			return sensing_interval_;
		
		case CONTINUOUS:
			/// If it is a continuous sensing activity, get the expire timer 
			/// from a normal distribution (limits between 0 and 1)
			return CONTINUOUS_TIME;
		
		case ON_DEMAND:
			return -1;
		
		case EVENT_DRIVEN:
			/// If the sensing activity was defined as EVENT_DRIVEN, it should
         	/// be replaced by the CONTINUOUS one
			return CONTINUOUS_TIME;

		default: fprintf(stderr,"Unrecognized sensing type!\n");
			abort();
	}
}

/// Shedule the first sensing event if this activity type isn�t the continuous 
/// one.
void DataGenerator::start()
{
	double sensT;
	sensT = getExpireTime();

	if(sensT != -1)
	{
	   sensTimer_->resched(sensT);
	}
}

/// Stop the object timer, dropping all the scheduled events.
void DataGenerator::stop()
{
	if (sensTimer_->status() == TIMER_PENDING)
	{
		sensTimer_->cancel();
	}
}

/// Insert a new SensorBaseApp to the DataGenerator SensorBaseApp list.
void DataGenerator::insertNewApp(SensorBaseApp* app)
{
	app_.push_back(app);
}

/// Returns DataGenerator application data type
AppDataType DataGenerator::type() const
{
	return type_;
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int DataGenerator::command(int argc, const char*const* argv)
{
	return TclObject::command(argc, argv);
}

/// Sets the sensing interval for programmed sensing.
void DataGenerator::setSensingInterval(double si)
{
	sensing_interval_=si;
}

/// Returns sensing interval for programmed sensing.
double DataGenerator::getSensingInterval()
{
	return sensing_interval_;
}

