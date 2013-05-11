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
/// Class that describes all the variables and functions of a sensor node. 
/// It can be used as an initial  approach to the development of new 
/// sensor nodes types.
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
#ifndef __ns_sensornode_h__
#define __ns_sensornode_h__

#include <stdlib.h>
#include <list>

#include "app.h"
#include "mobilenode.h"
#include "battery.h"

enum
{
	ON  = 0,
	OFF = 1
};

typedef list<Application*> AppList;

/// A class that represents a sensor node with its caracteristics and 
/// functionality. 
/// extends NS-2 MobileNode class
class SensorNode : public MobileNode
{
	friend class Battery;
	
	public:		
		
		/// Constructor.
		SensorNode();

		/// NS-2 function.
		virtual int command(int argc, const char*const* argv);

		/// Accessor methods.
		double sensingPower();
		double processingPower();
		int instructionsPerSecond();

	protected:

		/// List of applications associated with the node.
		AppList apps_;

		/// Indicates the number of instructions that the nodes processor can 
		/// execute per time unit (usually seconds).
		int instructionsPerSecond_;
			
		/// Indicates the energy consumption that a node has when it performs 
		/// its processing activity - thsi value should be in Joules/Second.
		double processingPower_;
		
		// Indicates the energy consumption that a node has, when it performs 
		// its sensing activity - this value should be in Joules/Second.
		double sensingPower_;

		// Indicates whether the sensing activity is in use or not
		int sensorUseState;

		// Indicates whether the processing activity is in use or not
		int processorUseState;

		// Indicates wheter the transceptor activity is in use or not
		int transceptorUseState;
	
		// Performs a test on the initial behavior of the node.
		virtual void selfTest();

		// Turns the node off when it is necessary.
		virtual void sleep();

		// Turns the node on when it is necessary.
		virtual void wakeUp();	
};

#endif

