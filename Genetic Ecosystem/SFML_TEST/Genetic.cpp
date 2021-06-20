#include "Genetic.h"

Genetic::Genetic()
{
	srand(time(NULL));
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
		sf::Creature new_creature;

		// Set generation
		new_creature.Get_Speed_Chromosome()->Set_Generation(current_generation);

		// Loop to add digits
		for (int j = 0; j < NUM_GENES; j++)
		{
			// Check random value from 1 to 100
			if ((rand() % 100 + 1) >= 50)
			{
				// Add a 1
				new_creature.Get_Speed_Chromosome()->Add_Gene('1');
			}
			else
			{
				// Add a 0
				new_creature.Get_Speed_Chromosome()->Add_Gene('0');
			}
		}

		// Add string to population
		population.push_back(new_creature);
	}

	// Increase Generation
	current_generation++;
}

void Genetic::Evaluate_Fitness()
{
	// Temp creature
	sf::Creature temp_creature;

	// Find maximums
	max_fir = &temp_creature;
	max_sec = &temp_creature;

	// Loop through the rest of the population
	for (int i = 0; i < population.size(); i++)
	{
		// Check if alive
		if (population[i].Is_Alive())
		{
			// Check if it is fitter (>= prioritizes new creatures
			if (population[i].Get_Food() >= max_fir->Get_Food())
			{
				max_sec = max_fir;
				max_fir = &population[i];
			}
			else if (population[i].Get_Food() >= max_sec->Get_Food())
			{
				max_sec = &population[i];
			}
		}
	}

	// Generate children from fittest
	Generate_Children();

	// Set temp creature to high food
	temp_creature.Add_Food(9999);

	// Find minimums
	min_fir = &temp_creature;
	min_sec = &temp_creature;

	// Loop through the rest of the population
	for (int i = 0; i < population.size() - 2; i++)
	{
		// Check if alive
		if (population[i].Is_Alive())
		{
			// Check if it is fitter
			if (population[i].Get_Food() < min_fir->Get_Food())
			{
				min_sec = min_fir;
				min_fir = &population[i];
			}
			else if (population[i].Get_Food() < min_sec->Get_Food())
			{
				min_sec = &population[i];
			}
		}
	}

	// Kill the weakest
	min_fir->Kill();
	min_sec->Kill();
}

void Genetic::Generate_Children()
{
	// Create new children
	sf::Creature child[2];

	// Set correct generation
	child[0].Get_Speed_Chromosome()->Set_Generation(current_generation);
	child[1].Get_Speed_Chromosome()->Set_Generation(current_generation);

	// Loop for all genes in a chromosome
	for (int i = 0; i < NUM_GENES; i++)
	{
		// If in first half
		if (i < (NUM_GENES / 2))
		{
			child[0].Get_Speed_Chromosome()->Add_Gene(max_fir->Get_Speed_Chromosome()->Get_Genes().at(i));
			child[1].Get_Speed_Chromosome()->Add_Gene(max_sec->Get_Speed_Chromosome()->Get_Genes().at(i));
		}
		else
		{
			child[0].Get_Speed_Chromosome()->Add_Gene(max_sec->Get_Speed_Chromosome()->Get_Genes().at(i));
			child[1].Get_Speed_Chromosome()->Add_Gene(max_fir->Get_Speed_Chromosome()->Get_Genes().at(i));
		}
	}

	// Loop for both children
	for (int i = 0; i < 2; i++)
	{
		// Mutation chance
		if ((rand() % 100 + 1) <= CHANCE)
		{
			// Mutate a random gene
			child[i].Get_Speed_Chromosome()->Mutate_Gene(rand() % NUM_GENES);
		}

		// Add children to population
		population.push_back(child[i]);
	}

	// Increase Generation
	current_generation++;
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
			if (population[i].Get_Speed_Chromosome()->Get_Generation() == display_gen)
			{
				cout << "Genes: " << population[i].Get_Speed_Chromosome()->Get_Genes() << " Fitness: " << population[i].Get_Speed_Chromosome()->Get_Fitness() << endl;
			}
		}

		// New line
		cout << endl;

	} while (display_gen < current_generation);
}
