#include "Chromosome.h"



Chromosome::Chromosome()
{
	genes = "";
	fitness = 0;
	generation = 0;
}

Chromosome::~Chromosome()
{

}

void Chromosome::Add_Gene(char new_gene)
{
	genes.push_back(new_gene);
}

void Chromosome::Calculate_Fitness()
{
	// Initialise fitness
	fitness = 0;

	// Loop through all genes
	for (int i = 0; i < NUM_GENES; i++)
	{
		if (genes.at(i) == '1')
		{
			// increase fitness
			fitness++;
		}
	}
}

void Chromosome::Mutate_Gene(int mutated_gene)
{
	// Change from 1 to 0 or vice versa
	if (genes.at(mutated_gene) == '1')
	{
		genes.at(mutated_gene) = '0';
	}
	else
	{
		genes.at(mutated_gene) = '1';
	}
}

void Chromosome::Set_To_Chromosome(Chromosome new_chromosome)
{
	// Loop through all genes
	for (int i = 0; i < NUM_GENES; i++)
	{
		genes[i] = new_chromosome.genes[i];
	}
}

void Chromosome::Set_Genes(string new_val)
{
	genes = new_val;
}

void Chromosome::Set_Fitness(int new_fit)
{
	fitness = new_fit;
}

void Chromosome::Set_Generation(int new_gen)
{
	generation = new_gen;
}

string Chromosome::Get_Genes()
{
	return genes;
}

int Chromosome::Get_Fitness()
{
	Calculate_Fitness();

	return fitness;
}

int Chromosome::Get_Generation()
{
	return generation;
}
