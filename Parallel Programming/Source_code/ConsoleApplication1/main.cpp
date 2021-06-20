#include <iostream>
#include <string>
#include "person.h"
#include <list>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <random>
#include <cchannel.h>
#include <thread>
#include <mutex>

using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

// Define the alias "the_clock" for the clock type we're going to use.
typedef std::chrono::steady_clock the_clock;

const int FORENAMES = 15;
const int SURNAMES = 15;

//function prototypes
void InitPeople(vector<person>&, string[], string[], mutex&);						//full list of people, possible forenames and surnames, people creation mutex
void ThreadFunction(vector<person>&, string[], string[], mutex&, int);					//full list of people, possible forenames and surnames, people creation mutex, ints for random seed
void SearchPeople(vector<person>, vector<person>&, int, int, int&, mutex&, int);		//full list of people, list of selected people, min of search, max of search, total people found, the counting mutex, chosen month
void AddtoSelected(vector<person>&, person, int&, mutex&);						//list of selected people, blank (temp) person, total people found, the counting mutex
void SortPeople(vector<person>&, string[], string[], int, int, bool&);		//list of selected people, possible forenames and surnames, indexes of the two people, swapped flag
void DisplayNames(vector<person>, int);											//sorted list of selected people, number of people found

void main()
{	
	//initialise the randomiser
	srand(time(NULL));

	//create the vector of people
	vector<person> people;				//full vector of people
	vector<person> selected;			//people with specified birth month
	mutex count_mutex;					//mutex to lock when counting number of people
	mutex people_mutex;					//locks to add to selected list
	int no_of_people = 0;				//the amount of people selected
	int chosen_month;					//the month to be searched for

	//create the possible names
	string forenames[FORENAMES] = { "Bill", "Tim", "George", "Frank", "Charlie", "Sarah", "Claire", "Suzie", "Kate", "Elizabeth", "Greg", "Caitlin", "Max", "Natalie", "Andrew"};
	string surnames[SURNAMES] = { "Smith", "Jones", "Taylor", "Williams", "Brown", "Davies", "Evans", "Wilson", "Thomas", "Roberts", "Balbirnie", "Rosser", "Mills", "Baker-Hall", "Milne"};

	//create the random vector of people
	//pass by reference so that the vector will stay as it is
	InitPeople(people, forenames, surnames, people_mutex);


	//create the threads
	vector<thread> searchThreads;
	
	//get specific month
	do
	{
		cout << "Which month would you like to search for? (Enter 1-12)" << endl;
		cin >> chosen_month;
		if (chosen_month < 1 || chosen_month > 12)
		{
			system("CLS");
			cout << "Invalid Entry" << endl;
		}
	} while (chosen_month < 1 || chosen_month > 12);

	//define threads
	for (int i = 0; i < people.size() / 100; i++)
	{
		//search for everyone with specific birth month
		searchThreads.push_back(thread(SearchPeople, people, ref(selected), (i * 100), ((i * 100) + 99), ref(no_of_people), ref(count_mutex), chosen_month));
	}

	//rejoin threads
	for (int i = 0; i < searchThreads.size(); i++)
	{
		searchThreads[i].join();
	}

	//vector of threads
	vector<thread> sortThreads;
	bool swapped;
	int startPoint = 0;

	//do until sorted
	do 
	{
		swapped = false;
		for (int i = startPoint; i < selected.size(); i += 2)
		{
			//sort the two that are assigned
			if ((i + 1) <= (selected.size() - 1))
			{
				sortThreads.push_back(thread(SortPeople, ref(selected), forenames, surnames, i, i + 1, ref(swapped)));
			}
		}
		
		for (int i = 0; i < sortThreads.size(); i++)
		{
			sortThreads[i].join();
		}
		
		sortThreads.clear();

		if (startPoint == 0)
		{
			startPoint = 1;
		}
		else if (startPoint == 1)
		{
			startPoint = 0;
		}

	} while (swapped == true);


	//display results
	DisplayNames(selected, no_of_people);
}

void InitPeople(vector<person> &people, string forenames[], string surnames[], mutex &people_mutex)
{
	thread init_threads[10];

	//create 100 threads
	for (int i = 0; i < 10; i++)
	{
		init_threads[i] = thread(ThreadFunction, ref(people), forenames, surnames, ref(people_mutex), i);
	}
	for (int i = 0; i < 10; i++)
	{
		init_threads[i].join();
	}
}

void ThreadFunction(vector<person> &people, string forenames[], string surnames[], mutex &people_mutex, int rando_seed)
{
	//create 100 people
	for (int k = 0; k < 100; k++)
	{
		//generate a random seed for the randomiser
		int seed = rando_seed + ((rando_seed + 1) * 10) + (k * 100);
		people_mutex.lock();
		people.push_back(person(forenames, surnames, seed));
		people_mutex.unlock();
	}
}

void SearchPeople(vector<person> people, vector<person> &selected, int min, int max, int &no_of_people, mutex &count_mutex, int chosen_month)
{
	//search for specific days
	for (int i = min; i < max; i++)
	{
		if (people[i].getDOBMonth() == chosen_month)
		{
			//add people born in september to new vector
			AddtoSelected(selected, people[i], no_of_people, count_mutex);
		}
	}
}

void AddtoSelected(vector<person> &selected, person newMemeber, int &no_of_people, mutex &count_mutex)
{
	selected.push_back(newMemeber);
	count_mutex.lock();
	no_of_people += 1;
	count_mutex.unlock();
}

void SortPeople(vector<person> &selected, string forenames[], string surnames[], int idx1, int idx2, bool &swapped)
{
	person temp(forenames, surnames, 0);	//blank person

	//check and swap
	if (selected[idx1].getSurname() > selected[idx2].getSurname())
	{
		temp = selected[idx1];
		selected[idx1] = selected[idx2];
		selected[idx2] = temp;

		swapped = true;
	}
}

void DisplayNames(vector<person> selected, int no_of_people)
{
	//display all the info
	for (int i = 0; i < selected.size(); i++)
	{
		cout << selected[i].getForename() << " " << selected[i].getSurname() << " " << selected[i].getDOBDay() << "/" << selected[i].getDOBMonth() << "/" << selected[i].getDOBYear() << endl;
	}
	cout << endl << "There are " << no_of_people << " people" << endl;
}