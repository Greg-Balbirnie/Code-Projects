#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "Chromosome.h"
#include "Genetic.h"

using namespace std;

Genetic genetic_algorithm;

void main()
{
	// Seed the randomiser
	srand(time(NULL));

	// Create the population
	genetic_algorithm.Generate_Population();

	// loop until fitness 8
	do
	{
		

		// Evaluate the fittness of each member
		genetic_algorithm.Evaluate_Fitness();

		// DEBUG show generation and fittness
		//cout << "Gen: " << current_generation << ", fittest: " << max_fir->Get_Fitness() << endl;

		if (genetic_algorithm.max_fir->Get_Fitness() == genetic_algorithm.NUM_GENES)
		{
			// DEBUG show last gen
			cout << "Solution found in generation " << genetic_algorithm.current_generation << endl;

			// Break out of while
			break;
		}
		else
		{
			// Move to next gen
			genetic_algorithm.current_generation++;

			// Create the children 
			genetic_algorithm.Generate_Children();
		}
	} while (true);

	// Display population
	genetic_algorithm.Show_Results();

	// Hang
	while (true)
	{

	}
}