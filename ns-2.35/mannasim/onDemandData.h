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
#ifndef on_demand_data_
#define on_demand_data_

/// Request type possible values. 
/// REAL - the node collects new data.
/// BUFFER - the node gets data from its buffer.
/// AVERAGE - the node gets data from the average of all data from its buffer.
enum
{
	REAL   =  0,
	BUFFER =  1,
	AVERAGE = 2
};

#include <common/ns-process.h>
#include "sensedData.h"

/// Class that encapsulates the request messages functionality.
///
/// extends SensedData
class OnDemandData : public SensedData{

	public:
		
		/// Constructors.
		OnDemandData();
		OnDemandData(int request_type);
		OnDemandData(OnDemandData &data);

		/// Size of the object.
		int size() const;
		
		/// Creates a copy of the object.
		AppData * copy();
		
		/// Returns the OnDemandData request type.
		int & requestType();
		
	private:
	
		/// Request type of OnDemandData. Possible values BUFFER,
		/// REAL and AVERAGE
		int request_type_;
		
		//n�o sei se precisa. Acho melhor colocar fun��o no SensedData 
		//que retornatamanho do infoRepository
		//indicates the number of parameters (OnDemandParameters)
		//	int parameters_amount_;
};


#endif
