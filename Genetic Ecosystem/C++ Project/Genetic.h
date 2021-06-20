#pragma once

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "Chromosome.h"

using namespace std;

class Genetic
{
public:
	Genetic();
	~Genetic();

	// Function Prototypes
	void Generate_Population();
	void Evaluate_Fitness();
	void Generate_Children();
	void Show_Results();


	// The number of genes per chromosome
	const int NUM_GENES = 8;

	// The percentage chance of mutation
	const int CHANCE = 10;

	// The full population
	vector<Chromosome> population;

	//Declare first and second fittest chromosome
	Chromosome* max_fir;
	Chromosome* max_sec;

	// The generation the algorithm is currently on
	int current_generation = 0;
};

