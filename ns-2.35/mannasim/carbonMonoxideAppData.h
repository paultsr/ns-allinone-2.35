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
#ifndef __carbonMonoxideAppData_h__
#define __carbonMonoxideAppData_h__

#include <common/ns-process.h>
#include <nr.hh>

#include "onDemandParameter.h"
#include "diffusion/nrAttributes.h"
#include "diffusion/attrsExporter.h"

/// Creates a carbon monoxide  application data type. This data type 
/// represents the pure data collect by the sensor node. This data
/// is sent to the processing module for futher treatment.
///
/// extends AppDataAttrsExporter
class CarbonMonoxideAppData : public AppDataAttrsExporter
{
	public:
	
		/// Constructor, parameters are carbon monoxide data collected
		/// and timestamp
		CarbonMonoxideAppData(double carbonMonoxideLevel, double sample_time); 
		
		/// Creates a copy of the object.
		AppData* copy();
		
		/// Size, in bytes, of the object.
		int size() const;
			
		/// Check if satisfies the condition especified by
		/// operation and value. It is used in ON DEMAND network
		bool compareData(AppData* data, int operation);
		
		/// Check if the value is greater than this one.
		/// It is used in EVENT DRIVEN network
        bool checkEvent(AppData* data_);
        
        /// Accessor methods
        double data();
		double time();
		
		void setData(double data);
		void setTime(double time);
	
 		/// Set attributes of sensed data. This is called by CommonNodeDiffApp.
		/// Changed by Julio Cesar e Melo.
		virtual void setAttributes(NRAttrVec * attrs);
    
	private:
	
		/// Carbon monoxide data.
		double data_;
		
		/// Timestamp for monoxide data. 
		double time_;
};

#endif
