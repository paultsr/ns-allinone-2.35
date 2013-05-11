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
/// Generates synthetic temperature data. Simulates a carbon monoxide sensor 
/// node sensing the environment.
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#include "carbonMonoxideDataGenerator.h"


/// This static class creates a link between the C++ class and the TCL script 
/// in the simulation scenario. Provides an instance of  
/// CarbonMonoxideDataGenerator class in the TCL simulation script. 
static class CarbonMonoxideDataGeneratorClass : public TclClass
{
	public:
		CarbonMonoxideDataGeneratorClass() : TclClass(
				"DataGenerator/CarbonMonoxideDataGenerator"){}
		TclObject* create(int, const char*const*)
		{
			return (new CarbonMonoxideDataGenerator);
		}

}class_carbon_monoxide_data_generator;

/// Constructor, binds TCL script parameters to C++ code variables and
/// creates a disseminating timer.
CarbonMonoxideDataGenerator::CarbonMonoxideDataGenerator() : 
								DataGenerator(CARBON_MONOXIDE_SENSED_DATA)
{
	// Creates the random number that that generates the syntectic data.
	rand_ = new RNG(RNG::HEURISTIC_SEED_SOURCE, 1);
	
	/// Get from the TCL script: data average, standard deviation, and maximum
	/// Carbon Monoxide value allowed.
	bind("avg_measure",&avg_measure);
	bind("std_deviation",&std_deviation);
	bind("maximumCarbonMonoxideValue",&maximumCarbonMonoxideValue);
}

/// Carbon monoxide sensing simulation. This is the most important function 
/// since it creates the random data, encapsulates it on a AppData type and 
/// return it to the sensor node.
AppData* CarbonMonoxideDataGenerator::collect()
{
	double monoxide = rand_->normal(avg_measure, std_deviation);

	printf("Carbon Monoxide Data %f - Time %f\n",
										monoxide,Scheduler::instance().clock());
	
	CarbonMonoxideAppData* data = new CarbonMonoxideAppData(
										monoxide, Scheduler::instance().clock());
	
	/// Returns an application data that contains the carbon monoxide collect 
	/// value
	return data;
}

/// NS-2 command function overloaded. Deals with TCL script commands to C++ 
/// implementation.
int CarbonMonoxideDataGenerator::command(int argc, const char*const* argv)
{
	if(argc == 3){
		if(strcmp("set-event",argv[1]) == 0)
		{
			avg_measure = atof(argv[2]);
			printf("EVENT: New avg_measure: %.3f\n",atof(argv[2]));
			return TCL_OK;
		}
	}
	return DataGenerator::command(argc, argv);
}

/// Returns average measure for synthetic carbon monoxide generation.
double CarbonMonoxideDataGenerator::getAvgMeasure()
{
	return avg_measure;
}

/// Sets average measure value for synthetic carbon monoxide generation.
void CarbonMonoxideDataGenerator::setAvgMeasure(double avg_measure)
{
	this->avg_measure = avg_measure;
}

/// Returns the maximum allowed carbon monoxide value.
CarbonMonoxideAppData* CarbonMonoxideDataGenerator::getMaximumAllowedValue()
{
	return new CarbonMonoxideAppData(maximumCarbonMonoxideValue,0.0);
}
