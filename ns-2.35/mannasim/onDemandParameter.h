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
#ifndef on_demand_parameter_h_
#define on_demand_parameter_h_

/// Comparation operations supported.
enum
{
	GREATER_THAN = 0,
	LESS_THAN 	 = 1,
	EQUAL 		 = 2
};

#include <common/ns-process.h>

/// Represents a query (each item) of the request message (OnDemandData).
///
/// extends AppData
class OnDemandParameter : public AppData
{
	public:
	
		/// Constructor.
		OnDemandParameter(AppData* data, int operation);
		
		/// Returns the size of the object.
		int size() const;
		
		/// Creates a copy of the object.		
		AppData * copy();
		
		/// Accessor methods
		int & operation();
		AppData * data();
	
	private:
		
		/// Data requested.
		AppData* data_; 
		
		/// Type of operation. Supported values nowadays:
		/// GREATER_THAN, LESS_THAN, EQUAL
		int operation_;  
};

#endif
