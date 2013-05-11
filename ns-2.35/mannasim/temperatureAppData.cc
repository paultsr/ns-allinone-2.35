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
/// Defines a temperature application data type. Encapsulates the 
/// raw data from DataGenerator.
/// 
/// authors: Linnyer B. Ruiz
///	         Fabricio A. Silva
///			 Thais Regina de M. Braga 
///  		 Kalina Ramos Porto
/// 		 Julio Cesar e Melo
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
#include "temperatureAppData.h"

/// TemperatureAppData constructor. Sets temperature and colleted 
/// time for the object, also calls super class constructor AppDataAttrsExporter() 
/// with AppDataType temperature.
TemperatureAppData::TemperatureAppData(double temperature, double sample_time) :
								 AppDataAttrsExporter(TEMPERATURE_SENSED_DATA)
{
	data_ = temperature;
	time_ = sample_time;
}

/// Checks if the value of the current object of this class is greater 
/// than the value of the parameter object. Until now, we only implemented
/// the GREATER THAN  option for EVENT DRIVEN network.
bool TemperatureAppData::checkEvent(AppData* data_)
{
	double value = ((TemperatureAppData*)data_)->data();

	if(value >= this->data())
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// Check if current value satisfies the condition especified by operation 
/// and data parameters. This method is used in ON DEMAND network.
bool TemperatureAppData::compareData(AppData* data, int operation)
{
  double value = ((TemperatureAppData*)data)->data();

	switch (operation)
	{
		case GREATER_THAN:
			return (value > this->data());

		case LESS_THAN:
			return (value < this->data());
 
		case EQUAL:
			return (value == this->data());

		default:
			fprintf(stderr,"TemperatureAppData::compareData - Invalid Operation.\n");
			return false;
	}
}

/// Sets atributes about temperature sensed data. Called by 
/// CommonNodeDiffApp (directed difusion).
/// 
/// Created by Julio Cesar e Melo
void TemperatureAppData::setAttributes(NRAttrVec * attrs)
{
	attrs->push_back(
		SensedValueAttr.make(NRAttribute::IS, this->data()));
}

/// Returns the size of the temperatureAppData object. Size in bytes.
int TemperatureAppData::size() const
{
	return sizeof(TemperatureAppData);
}

/// Creates a copy of the temperatureAppData object.
AppData * TemperatureAppData::copy()
{
	return new TemperatureAppData(data_, time_);
}

/// Returns temperature data.
double TemperatureAppData::data()
{
	return data_;
}

/// Ajusts temperature data.
void TemperatureAppData::setData(double data)
{
	data_ = data;
}
	
///	Returns timestamp for temperature data.
double TemperatureAppData::time()
{
	return time_;
}

/// Ajusts timestamp for temperature data.
void TemperatureAppData::setTime(double time)
{
	time_ = time;
}
/// Returns temperature data priority.
void TemperatureAppData::setPriority(int p)
{
	priority_ = p;
}

/// Ajust temperature data priority.
int & TemperatureAppData::getPriority()
{
	return priority_;
}
