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
/// Generates synthetic carbon monoxide data. Simulates a temperature sensor 
/// node sensing the environment.
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
/// 
#ifndef __carbon_monoxide_data_generator__
#define __carbon_monoxide_data_generator__

#include "dataGenerator.h"
#include "carbonMonoxideAppData.h"

/// Generate synthetic carbon monoxide data. Synthetic data is based on average 
/// and standard deviation values for a normal probability distribution. A 
/// maximum allowed value should also be set.
/// extends DataGenerator
class CarbonMonoxideDataGenerator : public DataGenerator
{
	public:
		
		/// Constructor
		CarbonMonoxideDataGenerator();
		
		/// Data sensing simulation function
		AppData* collect();
		
		/// NS-2 function
		virtual int command(int argc, const char*const* argv);
		
		/// Accessor methods
		double getAvgMeasure();
		void setAvgMeasure(double avg_measure);
		virtual CarbonMonoxideAppData* getMaximumAllowedValue();
		
	protected:
		RNG* rand_;
		
		double avg_measure;
		double std_deviation;
		double maximumCarbonMonoxideValue;
};

#endif 
