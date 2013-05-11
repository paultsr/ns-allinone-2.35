#include "accounts.h"

/// 
Accounts * Accounts :: instance()
{
	static Accounts single_instance;
	return &single_instance;
}

Accounts::Accounts()
{
	nodes = 0;
	groups_counter = 0;
	nodes_per_group = 0;
	consumed_energy = 0;
	management_msgs_sent  = 0;
	negotiation_msgs_sent = 0;
}

void Accounts::incr_sent_msgs()
{
	management_msgs_sent++;
}

void Accounts::incr_negotiation_msgs()
{
	negotiation_msgs_sent++;
}

void Accounts::incr_nodes(int number_of_nodes)
{
	nodes += number_of_nodes;
}

void Accounts::incr_energy_consumption(double consumption)
{
	consumed_energy += consumption;
}

void Accounts::incr_nodes_per_group(int number_of_nodes)
{
	nodes_per_group += number_of_nodes;
	groups_counter++;
}

void Accounts::imprime(char * output_dir)
{		
	char * ophans = new char [60];
	char * energy = new char [60];
	char * sent_msgs = new char [60];
	char * negotiation_msgs = new char [60];

	strcpy(ophans, output_dir);
	strcpy(energy, output_dir);
	strcpy(sent_msgs, output_dir);	
	strcpy(negotiation_msgs, output_dir);
	
	/* imprime em arquivo as msg enviadas */
	FILE *sent_msgs_file;
	
	strcat(sent_msgs, "/management_msgs_sent.dat");
	sent_msgs_file = fopen (sent_msgs, "a");
	
	fprintf(sent_msgs_file, "%d\n", management_msgs_sent);
	fclose(sent_msgs_file);
	/******************************************************/

	/* imprime em arquivo a quantidade de nos �rf�os */
	FILE * orphans_file;
	
	strcat(orphans, "/ophan_nodes.dat");
	orphans_file = fopen (orphans, "a");
	
	fprintf(orphans_file, "%d\n", (NUMERO_NOS_COMUNS - nodes));
	fclose(orphans_file);
	/******************************************************/

	/* imprime em arquivo a energia consumida, na m�dia, pelos CHs */
	FILE * energy_file;
	
	strcat(energy, "/consumed_energy.dat");
	energy_file = fopen(energy, "a");
	
	fprintf(energy_file, "%.3f\n", consumed_energy);
	fclose(energy_file);

	/* imprime em arquivo as msg enviadas */
	FILE * negotiation_msgs_file;
	
	strcat(negotiation_msgs, "/negotiation_msgs_sent.dat");
	negotiation_msgs_file = fopen (negotiation_msgs, "a");
	
	fprintf(negotiation_msgs_file, "%d\n", negotiation_msgs_sent);
	fclose(negotiation_msgs_file);
}

