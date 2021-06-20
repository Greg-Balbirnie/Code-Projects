#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "Creature.h"

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

	// The full population
	vector<sf::Creature> population;

private:
	// The number of genes per chromosome
	const int NUM_GENES = 16;

	// The percentage chance of mutation
	const int CHANCE = 100;

	//Declare first and second fittest chromosome
	sf::Creature* max_fir;
	sf::Creature* max_sec;

	//Declare first and second least fittest chromosome
	sf::Creature* min_fir;
	sf::Creature* min_sec;

	// The generation the algorithm is currently on
	int current_generation = 0;
};

