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
/// This class represents data requests from an autosider of the Wireless  
/// Sensor Network. This kind of request is interesting for on demand  data
/// dissemination where dissemination only occurs when the outsider request
/// data.
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
#include "onDemandData.h"

/// For Mannasim in NS2.35; inserted by Paul S : http://paultsr.in
/// Default constructor, invokes the request_type parameter constructor
/// with default value.
//OnDemandData :: OnDemandData()
//{
	/// REAL request type is default.
//	OnDemandData::OnDemandData(REAL); 
//}
			
/// Constructor. The request type of the OnDemandData is passed as
/// parameter. Note that the superclass constructor (SensedData) is also
/// called with ON_DEMAND_DATA AppDataType.
OnDemandData :: OnDemandData(int request_type) : SensedData(ON_DEMAND_DATA) 
{
	this->request_type_ = request_type;
}

/// Copy constructor. The OnDemandData is passed as parameter.
OnDemandData :: OnDemandData(OnDemandData & data) : SensedData(data)
{
	request_type_ = data.requestType();
}

/// Returns the size, in bytes, of the object.
int OnDemandData :: size() const
{
	return sizeof(OnDemandData);
}

/// Creates a copy of the OnDemandData object.
AppData * OnDemandData ::  copy()
{
	return new OnDemandData(*this);
}

/// Returns the address-of the request type field. Use this function carrefully.
int & OnDemandData :: requestType()
{
	return request_type_;
}
