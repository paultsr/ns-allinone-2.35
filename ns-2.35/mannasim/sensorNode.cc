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
#include "sensorNode.h"
 

// This static class creates a link between the C++ class and the TCL script 
// in the simulation scenario.
static class SensorNodeClass : public TclClass
{
	public:
		SensorNodeClass() : TclClass("Node/MobileNode/SensorNode"){}
		TclObject* create(int, const char*const*)
		{
			return (new SensorNode()); 
		}
}class_sensornode;


/// SensorNode constructor. Binds TCL script parameters to C++ code variables
/// and initialize all the three activites of a sensor node: sensing, processing
/// and disseminating.
SensorNode::SensorNode() : MobileNode()
{
	bind("sensingPower_",&sensingPower_);
	bind("processingPower_",&processingPower_);
	bind("instructionsPerSecond_",&instructionsPerSecond_);

	sensorUseState = ON;
	processorUseState = ON;
	transceptorUseState = ON;
}

// NS-2 function
int SensorNode::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();

	if(argc == 3){
		if(strcmp(argv[1],"add-app") == 0)
		{
			/// Attach an application to the node.
			Application* app = (Application*)TclObject::lookup(argv[2]);
			apps_.push_back(app);
			return TCL_OK;	
		}
	}
	return MobileNode::command(argc,argv);
}

/// Performs a selfTest on the sensor node checking its proper functioning. 
/// NOT YET IMPLEMENTED
void SensorNode::selfTest(){}

// Turns the sensor node off when necessary.
void SensorNode::sleep()
{
	/// Using the energy model pointer, turns off the node�s transceptor.
	((Battery*)energy_model())->setNodeOff();

	/// Stops all the applications related to the node.
	Tcl& tcl = Tcl::instance();
	AppList::iterator it;
	Application* app;
	
	for (it = apps_.begin(); it != apps_.end(); it++)
	{
		app = *it;
		tcl.evalf("%s stop",app->name());
	}

	/// Set "off" to all states of node activity (sensing, processing and 
	/// disseminating)
	sensorUseState = OFF;
	processorUseState = OFF;
	transceptorUseState = OFF;
}

/// Turns the sensor node on when necessary.
void SensorNode::wakeUp(){

	// Using the energy model pointer, turns on the sensor node transceptor.
	((Battery*)energy_model())->setNodeOn();

	// Starts all the applications related to the node.
	Tcl& tcl = Tcl::instance();
	AppList::iterator it;
	Application* app;
	
	for (it = apps_.begin(); it != apps_.end(); it++)
	{
		app = *it;
		tcl.evalf("%s start",app->name());
	}

	/// Set "on" to all states of node activity (sensing, processing and 
	/// disseminating)
	sensorUseState = ON;
	processorUseState = ON;
	transceptorUseState = ON;
}

/// Returns node�s sensing power in Joules/Second.
double SensorNode::sensingPower()
{
	return sensingPower_;
}

/// Returns node�s processing power in Joules/Second.
double SensorNode::processingPower()
{
	return processingPower_;
}

/// Returns node�s instruction per second number.
int SensorNode::instructionsPerSecond()
{
	return instructionsPerSecond_;
}

