#include "person.h"
#include <string>
#include <random>
#include <chrono>

using namespace std;

person::person(string forenames[], string surnames[], int seed) 
{
	srand(seed);

	rando = rand() % 15;
	forename = forenames[rando];

	rando = rand() % 15;
	surname = surnames[rando];

	rando = rand() % 28 + 1;
	dob[0] = rando;

	rando = rand() % 12 + 1;
	dob[1] = rando;

	rando = rand() % 55 + 1945;
	dob[2] = rando;
}

person::~person()
{
}

string person::getForename()
{
	return forename;
}

string person::getSurname()
{
	return surname;
}

int person::getDOBDay()
{
	return dob[0];
}

int person::getDOBMonth()
{
	return dob[1];
}

int person::getDOBYear()
{
	return dob[2];
}