#pragma once
#include <string>

using namespace std;

class person
{
public:
	person(string [10], string [10], int);	//initialise the people
	~person();								//destructor
	string getForename();					//returns their forename
	string getSurname();					//returns their surname
	int getDOBDay();						//returns the day of their DOB
	int getDOBMonth();						//returns the month of their DOB
	int getDOBYear();						//returns the year of their DOB
private:
	string forename;						//first name
	string surname;							//second name
	int dob[3];								//date of birth
	int rando;								//random number
};

