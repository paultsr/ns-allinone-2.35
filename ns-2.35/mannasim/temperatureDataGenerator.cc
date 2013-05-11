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
/// Generates synthetic temperature data. Simulates a temperature sensor node
/// sensing the environment.
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#include "temperatureDataGenerator.h"

/// This static class creates a link between the C++ class and the TCL script 
/// in the simulation scenario. Provides an instance of the 
/// TemperatureDataGenerator class in the TCL simulation script. 
static class TemperatureDataGeneratorClass : public TclClass
{
	public:
		TemperatureDataGeneratorClass() : TclClass(
			"DataGenerator/TemperatureDataGenerator"){}
		TclObject* create(int, const char*const*)
		{
			return (new TemperatureDataGenerator);
		}

}class_temperature_data_generator;

/// Constructor, binds TCL script parameters to C++ code variables and
/// creates a disseminating timer.
TemperatureDataGenerator::TemperatureDataGenerator() : 
							DataGenerator(TEMPERATURE_SENSED_DATA)
{

	// Creates the random number that that generates the syntectic data.
	rand_ = new RNG(RNG::HEURISTIC_SEED_SOURCE, 1);
	
	/// Get from the TCL script: data average, standard deviation, and maximum
	/// Temperature value allowed.
	bind("avg_measure",&avg_measure);
	bind("std_deviation",&std_deviation);
	bind("maximumTemperatureValue",&maximumTemperatureValue);
}

/// Temperature sensing simulation. This is the most important function since it
/// creates the random data, encapsulates it on a AppData object and return it
/// to the sensor node.
AppData* TemperatureDataGenerator::collect()
{
	double t = rand_->normal(avg_measure, std_deviation);

	printf("Temperature Data %f - Time %f\n",t,Scheduler::instance().clock());
	TemperatureAppData* data = 
		new TemperatureAppData(t, Scheduler::instance().clock());
	
	/// Returns an application data, that contains the temperature collected 
	/// value
	return data;
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int TemperatureDataGenerator::command(int argc, const char*const* argv)
{
	if(argc == 3)
	{
		if(strcmp("set-event",argv[1]) == 0)
		{
			avg_measure = atof(argv[2]);
			printf("EVENT: New avg_measure: %.3f\n",atof(argv[2]));
			return TCL_OK;
		}
   }
   return DataGenerator::command(argc, argv);
}

/// Returns average measure for synthetic temperature generation.
double TemperatureDataGenerator::getAvgMeasure()
{
	return avg_measure;
}

/// Sets average measure value for synthetic temperature generation.
void TemperatureDataGenerator::setAvgMeasure(double avg_measure)
{
	this->avg_measure = avg_measure;
}

/// Returns the maximum allowed temperature value.
TemperatureAppData* TemperatureDataGenerator::getMaximumAllowedValue()
{
	return new TemperatureAppData(maximumTemperatureValue,0.0);
}
