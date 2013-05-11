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
/// This class represents the queries submited within a OnDemandData 
/// request message. Contains the requested data and a set of operations
/// allowed to be performed to get the data (for example, get data if 
/// temperature is GREATER_THAN 50 celsius degrees).
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
#include "onDemandParameter.h"

/// Constructor. Requested data and operation performed on it are passed
/// as parameters.
OnDemandParameter :: OnDemandParameter(AppData* data, int operation)
									 : AppData(ON_DEMAND_PARAMETER)
{
	this->data_ = data;
	this->operation_ = operation;
}
		
/// Returns OnDemandParameter object size in bytes.
int OnDemandParameter :: size() const
{
	return sizeof(OnDemandParameter);
}

/// Creates a copy of OnDemandParameter object.
AppData * OnDemandParameter :: copy()
{
	return new OnDemandParameter(data_, operation_);
}

/// Returns the address of operation_ field. Use this function carrefully.
int & OnDemandParameter :: operation()
{
	return operation_;
}

/// Returns the requested data.
AppData * OnDemandParameter :: data()
{
	return data_;
}