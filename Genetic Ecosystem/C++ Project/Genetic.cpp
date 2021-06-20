#include "Genetic.h"



Genetic::Genetic()
{
}


Genetic::~Genetic()
{
}

void Genetic::Generate_Population()
{
	// Loop to create 4 members
	for (int i = 0; i < 4; i++)
	{
		// Create a new member
		Chromosome new_mem;

		// Set generation
		new_mem.Set_Generation(current_generation);

		// Loop to add digits
		for (int j = 0; j < NUM_GENES; j++)
		{
			// Check random value from 1 to 100
			if ((rand() % 100 + 1) >= 50)
			{
				// Add a 1
				new_mem.Add_Gene('1');
			}
			else
			{
				// Add a 0
				new_mem.Add_Gene('0');
			}
		}

		// Add string to population
		population.push_back(new_mem);
	}
}

void Genetic::Evaluate_Fitness()
{
	// Find maximums
	max_fir = &population[0];
	max_sec = &population[1];

	// Switch first and second if they are wrong
	if (max_sec->Get_Fitness() > max_fir->Get_Fitness())
	{
		Chromosome* placeholder;

		placeholder = max_fir;
		max_fir = max_sec;
		max_sec = placeholder;
	}

	// Loop through the rest of the population
	for (int i = 2; i < population.size(); i++)
	{
		// Check if it is fitter
		if (population[i].Get_Fitness() >= max_fir->Get_Fitness())
		{
			max_sec = max_fir;
			max_fir = &population[i];
		}
		else if (population[i].Get_Fitness() >= max_sec->Get_Fitness())
		{
			max_sec = &population[i];
		}
	}
}

void Genetic::Generate_Children()
{
	// Create new children
	Chromosome child[2];

	// Set correct generation
	child[0].Set_Generation(current_generation);
	child[1].Set_Generation(current_generation);

	// Loop for all genes in a chromosome
	for (int i = 0; i < NUM_GENES; i++)
	{
		// If in first half
		if (i < (NUM_GENES / 2))
		{
			child[0].Add_Gene(max_fir->Get_Genes().at(i));
			child[1].Add_Gene(max_sec->Get_Genes().at(i));
		}
		else
		{
			child[0].Add_Gene(max_sec->Get_Genes().at(i));
			child[1].Add_Gene(max_fir->Get_Genes().at(i));
		}
	}

	// Loop for both children
	for (int i = 0; i < 2; i++)
	{
		// Mutation chance
		if ((rand() % 100 + 1) <= CHANCE)
		{
			// Mutate a random gene
			child[i].Mutate_Gene(rand() % NUM_GENES);
		}

		// Add children to population
		population.push_back(child[i]);
	}
}

void Genetic::Show_Results()
{
	// The generation to display
	int display_gen = -1;

	// Loop until display gen is outwith total gens
	do
	{
		// Start at 0, increase each time
		display_gen++;

		cout << "Generation " << display_gen << ":" << endl;

		// Loop through all of population
		for (int i = 0; i < population.size(); i++)
		{
			// Check if correct generation
			if (population[i].Get_Generation() == display_gen)
			{
				cout << "Genes: " << population[i].Get_Genes() << " Fitness: " << population[i].Get_Fitness() << endl;
			}
		}

		// New line
		cout << endl;

	} while (display_gen < current_generation);

	//// Display population
	//for (int i = 0; i < population.size(); i++)
	//{
	//	cout << population[i].Get_Genes() << endl;
	//}

	//cout << endl;

	//// Show all fitnesses
	//for (int i = 0; i < 4; i++)
	//{
	//	cout << population[i].Get_Fitness() << endl;
	//}
}