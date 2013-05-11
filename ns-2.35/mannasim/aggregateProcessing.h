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
/// Class that simulates the aggregating processing activity on sensed data. 
/// It's a specialization of processing class. 
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
#ifndef __aggregate_processing__
#define __aggregate_processing__

#include "processing.h"

/// Specialize Processing class adding aggregation to processing module.
///
/// extends Processing
class AggregateProcessing : public Processing
{
	public:
	
		/// Constructor
		AggregateProcessing();
		
		/// Simulates sensed data processing. Used in continous and
		/// programmed data sensing.
		virtual void processSensedData(AppData* data_);
		
		/// Receives data from multiple source and aggregates them.
		virtual void recvData(AppData* data_);
};

#endif

