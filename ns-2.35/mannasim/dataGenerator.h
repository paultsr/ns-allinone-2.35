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
#ifndef __data_generator__
#define __data_generator__

#include <list>
#include <tools/rng.h>
#include <common/ns-process.h>

#include "battery.h"
#include "sensorNode.h"
#include "sensorBaseApp.h"
#include "util/accounts.h"

/// Time interval for continous sensing. Continuity is simulated by reschedule 
/// the timer with a very small time.
#define CONTINUOUS_TIME 0.01

class DataGenerator;
class SensorBaseApp;

/// Type definition for a SensorBaseApp STL list.
typedef list<SensorBaseApp*> SensorAppList;

/// Sensing timer used to collect data.
class SensingTimer : public TimerHandler
{
	public:
		SensingTimer(DataGenerator* gen) : TimerHandler(), gen_(gen) {}
		inline virtual void expire(Event*);

	protected:
		
		/// DataGenerator used to "create" sensed data.
		DataGenerator* gen_;
};

/// Timer that controls the time interval in which an event is valid.
class EventTimer : public TimerHandler
{
	public:
		EventTimer(DataGenerator* gen) : TimerHandler(), gen_(gen) {}
		inline virtual void expire(Event*);

	protected:
	
		/// DataGenerator used to "create" sensed data.	
		DataGenerator* gen_;
};

/// Base class for sensed data creator objects. Specialized data should extends
/// this class.
class DataGenerator : public TclObject
{
	public:
		/// Constructors
	  	DataGenerator(){}
	  	DataGenerator(AppDataType type);

		/// NS-2 Function
		virtual int command(int argc, const char*const* argv);
	
		/// Control functions for simulation.
		void start();
		void stop();
		
		void generateData();
		void insertNewApp(SensorBaseApp* app);	
		virtual AppData* collect()
        {
           printf("DataGenerator::collect() - specialize this function\n");
        }

		/// Accessor methods
		AppDataType type() const;
		double getSensingInterval();
		void setSensingInterval(double si);

		friend class EventTimer;
		friend class SensingTimer;
		
	protected:
		
		/// Indentify the generator according to the AppData
		AppDataType type_;

		/// List of applications associated with 
		SensorAppList app_;

		/// Sensing timer.
		SensingTimer* sensTimer_;

		/// Sensing interval for programmed sensing. For continous sensing use
		/// CONTINUOUS_TIME constante.
		double sensing_interval_;

		/// Define the way the sensor device is supposed to generate data.
		int sensing_type_;

		// Function that returns the timer expire value, taking into account
		// the sensing activity type
		double getExpireTime();

		/// Insert erros into generated data. NOT IMPLEMENTED YET
		virtual void insertInterference();

		virtual AppData* getMaximumAllowedValue()
        {
            printf("DataGenerator::getMaximumAllowedValue() - specialize this function\n");
        }
        
};

#endif
