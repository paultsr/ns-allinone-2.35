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
#ifndef __ns_battery_h__
#define __ns_battery_h__

#include <node.h>
#include <energy-model.h>
#include <wireless-phy.h>

/* This class represents the sensor node battery. It inherits from
the NS-2 EnergyModel*/

/// Represents sensor node batery. Include method to turn the node on and off
/// put it to sleep and make it wake up. Also contains a method to decrease 
/// energy when work is done by the sensor.
///
/// extends EnergyModel
class Battery : public EnergyModel{

  public:
    
    /* Construtor: invoke the EnergyModel constructor with the need parameters */
    /// Constructor
    Battery(MobileNode* node, double energy, double l1, double l2);

    /* fun��o que decrementa a energia gasta com sensoriamento:
     * par�metros: tempo de sensoriamento e pot�ncia (W) do sensor
     * */
    virtual void DecrSensingEnergy(double sensing_time, double sensing_power);

    /* fun��o que decrementa a energia gasta com processamento 
     * par�metros: n�mero de instru��es realizadas e pot�ncia (W) 
		 * do processador
     * */
    virtual void DecrProcessingEnergy(int number_instructions, double instructions_per_second, double processing_power);

	/// Turn node On.
    virtual void setNodeOn();
    
	/// Turn node Off.
    virtual void setNodeOff();

	/// Put node in Sleep mode.
    virtual void sleep();
    
    /// Wake up the sleepy node.
    virtual void wakeUp();
};

#endif 
