#pragma once

#include <string>

using namespace std;

class Chromosome
{
public:
	Chromosome();
	~Chromosome();

	void Add_Gene(char);
	void Calculate_Fitness();
	void Mutate_Gene(int);

	// Setters
	void Set_Genes(string);
	void Set_Fitness(int);
	void Set_Generation(int);

	//Getters
	string Get_Genes();
	int Get_Fitness();
	int Get_Generation();

private:
	string genes;
	int fitness;
	int generation;

	const int NUM_GENES = 8;
};

