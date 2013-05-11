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
#include "aggregateProcessing.h"

/// This static class creates a link between the C++ class and the TCL script 
/// in the simulation scenario. Provides an instance of the 
/// AggregateProcessing class in the TCL simulation script. 
static class AggregateProcessingClass : public TclClass
{
	public:
		AggregateProcessingClass() : TclClass("Processing/AggregateProcessing"){}
		TclObject* create(int, const char*const*){
			return (new AggregateProcessing);
		}

}class_aggregate_processing;

/// Default construtor, clear processed data buffer (info_)
AggregateProcessing::AggregateProcessing() : Processing()
{
	info_ = NULL;
}

/// Simulates sensed data aggregating processing. The parametric data
/// is added in processed data buffer and energy is decresed from node 
/// battery.
void AggregateProcessing::processSensedData(AppData* data_)
{
	if (info_ == NULL)
	{
		info_ = new SensedData;
	}
		
	info_->insertNewData(data_->copy());

	// The activity above wastes sensor node energy! PROCESSING WASTE HERE!
	((Battery*)sensor_node_->energy_model())->DecrProcessingEnergy(
												AGGREGATION_INSTRUCTIONS,
												sensor_node_->instructionsPerSecond(),
												sensor_node_->processingPower());
}

/// Receives data from multiple sources and aggregates them. Useful
/// for cluster head based networks.
void AggregateProcessing::recvData(AppData* data_)
{
	if (info_ == NULL)
	{
		info_ = new SensedData;
	}
	
	AppDataList list = ((SensedData*)data_)->getData();
	AppDataList::iterator it;

	/// Aggregates all sensed data
	for (it = list.begin(); it != list.end(); it++)
	{
		info_->insertNewData(*it);
	}

	// The activity above wastes sensor node energy! PROCESSING WASTE HERE!
	((Battery*)sensor_node_->energy_model())->DecrProcessingEnergy(
												AGGREGATION_INSTRUCTIONS,
												sensor_node_->instructionsPerSecond(),
												sensor_node_->processingPower());
}

