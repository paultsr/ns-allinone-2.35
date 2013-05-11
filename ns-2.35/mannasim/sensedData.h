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
/// This class represents sensing applications data. It is derived 
/// from the NS-2 AppData. It includes methods to deal with messages
/// type, source node identification, message copy, message log 
/// management.
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
#ifndef __sensed_data__
#define __sensed_data__

#include <list>
#include <vector>
#include <scheduler.h>
#include <common/ns-process.h>


/// Databuffer size. Indicates how many samples can be stored.
#define BUFFER_SIZE 100

typedef list<int> GroupList;
typedef vector<AppData*> AppDataList;

/// The sensedData class encapsulates all information that needs to be
/// fowarded by the node.
/// extends NS-2 AppData
class SensedData : public AppData
{
	public:
	
		/// Contructors
		SensedData();
		SensedData(AppDataType type);
		SensedData(SensedData &data);

		/// Accessor Methods
		AppDataList getData();

		int& msgType();
		int& node_id();
		int& eventType();
		double & timeStamp();

		int priority();
		void set_priority(int p);
		AppData* copy();

		/// Inserts a new AppData in infoRepository
		void insertNewData(AppData* data);

		/// Informs if the infoRepository is empty or not
		bool existsData();

		/// infoRepository managment functions
		int size();
		int msgSize();
		int count();
		void clear();

	private:

		/// Message type
		int msgType_;

		/// Event type
		int eventType_;

		/// Source node identification
		int node_id_;

		double timestamp_;

		/// Message priority
		int priority_;

		/// Data log from messages
		AppDataList infoRepository;
};

#endif 

