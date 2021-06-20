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
	void Set_To_Chromosome(Chromosome);

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

	// TODO
	const int NUM_GENES = 16;
};

