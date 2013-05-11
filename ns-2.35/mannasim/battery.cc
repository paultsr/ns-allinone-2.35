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
/// This class represents the power supply of the node. It is derived from
/// NS-2 Energy model.
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
#include "battery.h"

/// This static class creates a link between the C++ class and the TCL script 
/// in the simulation scenario. Provides an instance of the 
/// Battery class in the TCL simulation script. 
static class BatteryClass : public TclClass
{
	public:
		BatteryClass ():TclClass ("EnergyModel/Battery") {}
		TclObject *create (int argc, const char *const *argv){
			if (argc == 8)
			{
				MobileNode *n=(MobileNode*)TclObject::lookup(argv[4]);
				return (new Battery(n, 
									atof(argv[5]), 
									atof(argv[6]), 
									atof(argv[7])));
			}
			else
			{
				Tcl::instance().add_error("Wrong arguments to Battery");
				return 0;
			}
        }
} class_battery;

/// Constructor. Invoke EnergyModel constructor with need parameters.
Battery::Battery(MobileNode* node, double energy, double l1, double l2) : 
										EnergyModel(node, energy, l1, l2)
{
}

/// Computes the energy consumed when the node is sensing data. 
void Battery::DecrSensingEnergy(double sensing_time, double sensing_power)
{
	double consumed_energy = sensing_power * sensing_time;
	
	/// The energy_ variable indicates the amount of energy of the node. 
	/// This variable is defined at EnergyModel class. 
	if (energy_ <= consumed_energy)
	{
		energy_ = 0.0;
	}
	else
	{
		energy_ = energy_ - consumed_energy;
	}
}

/// Computes the energy consumed when the node is processing data. 
void Battery::DecrProcessingEnergy(int number_instructions, double instructions_per_second, double processing_power)
{
	double processing_time;
	double consumed_energy;
	
	if (instructions_per_second == 0)
	{
		fprintf(stderr,"Battery::DecrProcessingEnergy - Division by zero!!!\n");
		abort();
	} 
 	
 	processing_time = ((double)(number_instructions))/instructions_per_second;
	consumed_energy = processing_power * processing_time;
	
	/// The energy_ variable indicates the amount of energy of the node. 
	/// This variable is defined at EnergyModel class. 
	if (energy_ <= consumed_energy)
	{
		energy_ = 0.0;
	}
	else
	{
		energy_ = energy_ - consumed_energy;
	}
}

/// Turn senso node On.
void Battery::setNodeOn(){
	node_on_ = true;
}

/// Turn the node Off
void Battery::setNodeOff(){
	node_on_ = false;
}

/// Put node in sleep mode.
void Battery::sleep()
{
	struct if_head head;

	head = ((Node *) node_)->ifhead();

	for (Phy * wp = head.lh_first; wp; wp = wp->nextnode())
	{
		((WirelessPhy *) wp)->node_off();
	}	
}

/// Wake up sleppy sensor node.
void Battery::wakeUp()
{
	struct if_head head;

	head = ((Node *) node_)->ifhead();

	for (Phy * wp = head.lh_first; wp; wp = wp->nextnode())
	{
		((WirelessPhy *) wp)->node_on();
	}
}
