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
/// 
/// authors: Helen Peters de Assunção
/// 		 Carlos Eduardo R. Lopes
///
/// 
///
/// --
/// The Manna Reseach Group
/// e-mail: mannateam@gmail.com
///
/// This project was financially supported by The National Council 
/// for Scientific and Technological Development (CNPq) from the 
/// brazilian government under the process number 55.2111/2002-3.
///
#ifndef __accounts__
#define __accounts__

#include <stdlib.h>
#include <stdio.h>
#include <string>

/* 
 * Classe com vari�veis que 
 * contabilizam as m�tricas 
 * avaliadas 
 * 
 */

/* ATEN��O: MUDARA QUANDO MUDAR O SCRIPT */
#define NUMERO_NOS_COMUNS 48
#define NUMERO_CLUSTER_HEADS 4

class Accounts
{
	public:

		//retorna a inst�ncia �nica
		Accounts * instance();

		void incr_sent_msgs();

		void incr_negotiation_msgs();
			
		void incr_nodes(int number_of_nodes);
		
		void incr_nodes_per_group(int number_of_nodes);

		void incr_energy_consumption(double consumption);
			
		void print_accounts(char * output_dir);
		
	protected:
	
		Accounts();
			
	private:

		/// Accounts the number of management messages sent
		/// by the node.
		int management_msgs_sent;

		/// Accounts the number of negotiation messages sent
		/// by the node.
		int negotiation_msgs_sent;

		/// Accounts the number of nodes that have a father, 
		/// the other nodes are orphans.
		int nodes;

		/// Average number of nodes per group (cluster)
		int nodes_per_group;
		
		/// Number of groups (cluster) in the network.
		int groups_counter;
			
		/// Cosumed energy with cluster head processing
		double consumed_energy;
};

#endif
