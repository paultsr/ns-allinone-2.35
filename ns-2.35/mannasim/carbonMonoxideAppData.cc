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
/// Defines a carbon monoxide application data type. Encapsulates the 
/// raw carbon monoxide data from DataGenerator.
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
#include "carbonMonoxideAppData.h"

/// CarbonMonoxideAppData constructor. Sets carbonMonoxideLevel and colleted 
/// time for the object, also calls super class constructor AppDataAttrsExporter() 
/// with AppDataType carbon monoxide.
CarbonMonoxideAppData::CarbonMonoxideAppData(double carbonMonoxideLevel, double sample_time) 
                       : AppDataAttrsExporter(CARBON_MONOXIDE_SENSED_DATA)
{
	data_ = carbonMonoxideLevel;
	time_ = sample_time;
}

/// Checks if the value of the current object of this class is greater 
/// than the value of the parameter object. Until now, we only implemented
/// the GREATER THAN  option for EVENT DRIVEN network.
bool CarbonMonoxideAppData::checkEvent(AppData* data_)
{
	/// Already know that AppData dynamic type is CarbonMonoxideAppData, 
	/// so perform the casting.
	double value = ((CarbonMonoxideAppData*)data_)->data();
	
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
bool CarbonMonoxideAppData::compareData(AppData* data, int operation)
{
	/// Already know that AppData dynamic type is CarbonMonoxideAppData, 
	/// so perform the casting.
	double value = ((CarbonMonoxideAppData*)data)->data();
	
	switch (operation)
	{
		case GREATER_THAN:
			return (value > this->data());
			
		case LESS_THAN:
			return (value < this->data());
		
		case EQUAL:
			return (value == this->data());
		
		default:
			fprintf(stderr,"CarbonMonoxideAppData::compareData - Invalid Operation.\n");
			return false;
	}
}

/// Sets atributes about carbon monoxide sensed data. Called by 
/// CommonNodeDiffApp (directed difusion).
/// 
/// Created by Julio Cesar e Melo
void CarbonMonoxideAppData::setAttributes(NRAttrVec * attrs)
{
	attrs->push_back(
		SensedValueAttr.make(NRAttribute::IS, this->data()));
}

/// Returns the size of the CarbonMonoxideAppData object. Size in bytes.
int CarbonMonoxideAppData::size() const
{
	return sizeof(CarbonMonoxideAppData);
}

/// Creates a copy of this CarbonMonoxideAppData object.
AppData * CarbonMonoxideAppData::copy()
{
	return new CarbonMonoxideAppData(data_, time_);
}

/// Returns carbon monoxide data.
double CarbonMonoxideAppData::data()
{
	return data_;
}

/// Ajusts carbon monoxide data.
void CarbonMonoxideAppData::setData(double data)
{
	data_ = data;
}

/// Returns carbon monoxide timestamp
double CarbonMonoxideAppData::time()
{
	return time_;
}

/// Ajusts carbon monoxide data timestamp.
void CarbonMonoxideAppData::setTime(double time)
{
	time_ = time;
}
