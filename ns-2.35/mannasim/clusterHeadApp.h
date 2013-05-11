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
/// This class simulates the behavior of a cluster head in a wireless
/// sensor network. Dispates its more capacity to process and disseminate
/// sensed data, a cluster head must also have a group of nodes under
/// its responsability - a child list.
///
/// authors: Linnyer B. Ruiz
///	         Fabr�cio A. Silva
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
#ifndef __cluster_head_h__
#define __cluster_head_h__

#include <list>
#include <stdio.h>
#include <common/ns-process.h>

#include "sensorBaseApp.h"

using namespace std;

typedef list<int> IdList;

/// Size of cluster head messages (in bytes)
#define CH_MSG_BYTES_ 64

/// Simulates the behavior of a wireless sensor network cluster head
/// device.
class ClusterHeadApp : public SensorBaseApp {

  public:

	/// Default Constructor
    ClusterHeadApp();

	/// Process the sensed data. Specialization of 
	/// Process::process_data() function. See NS-2 documentation
	/// for details.
    virtual void process_data(int size, AppData* data);

    virtual void processRequest(AppData* data);
  
  protected:
    
    /// Insert a new node in the child list.
    void insert_child(int id);
  
    /// Remove the node with addr = id in the child list.
    void remove_child(int id);

    /// Search for a node with addr = id in the child list.
    bool search_child(int id); 

    /// Disseminate data to the network.
    virtual void disseminateData();

  private:
  
  	/// Children list of the cluster head
    IdList child_list;
  
};

#endif
