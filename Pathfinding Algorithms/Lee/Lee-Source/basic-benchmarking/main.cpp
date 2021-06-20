//includes
#include<iostream>
#include<time.h> 
#include<list>
#include<chrono>

//namespace
using std::chrono::duration_cast;
using std::chrono::milliseconds;

using namespace std;

//the coordinate struct
struct coord
{
	int x;
	int y;
};

//function prototypes
void initialise(coord, coord);
void phase_1(coord, coord);
void phase_2(coord, coord);

//Width and Height constants
#define WIDTH 20
#define HEIGHT 20

//define the grid
int	grid[HEIGHT][WIDTH];

void main()
{
	//define start and end points as coordinates
	int rando;
	coord start;	
	coord end;	

	start.x = 0;
	start.y = 0;
	end.x = HEIGHT - 1;
	end.y = WIDTH - 1;

	initialise(start, end);		//initialise the grid
	
	phase_1(start, end);	//start phase 1
	phase_2(start, end);	//start phase 2
}

void initialise(coord start, coord end)
{
	//set up randomiser
	srand(time(NULL));
	int rando;

	//loop through width and height
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			rando = rand() % 10 + 1;
			if (rando == 5)
			{
				if (i != start.y || j != start.x)
				{
					if (i != end.y || j != end.x)
					{
						grid[i][j] = -2;	//randomly set some to -2
					}
				}
			}
			else
			{
				grid[i][j] = -1;	//set the rest to -1
			}
		}
	}
}

void phase_1(coord start, coord end)
{
	//start the number assignment
	grid[start.y][start.x] = 0;
	int n = 0;		//the current number
	
	//if end is unfound, keep searching
	while (grid[end.y][end.x] == -1)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				//when the current number is found, 
				//the surrounding points are checked for -1
				//and then changed if necessary
				if (grid[i][j] == n)
				{
					if (grid[i + 1][j] == -1)
					{
						if ((i + 1) < HEIGHT)
						{
							grid[i + 1][j] = n + 1;
						}
					}
					if (grid[i - 1][j] == -1)
					{
						if ((i - 1) >= 0)
						{
							grid[i - 1][j] = n + 1;
						}
					}
					if (grid[i][j + 1] == -1)
					{
						if ((j + 1) < WIDTH)
						{
							grid[i][j + 1] = n + 1;
						}
					}
					if (grid[i][j - 1] == -1)
					{
						if ((j - 1) >= 0)
						{
							grid[i][j - 1] = n + 1;
						}
					}
				}
			}
		}
		n++;	//increment n
	}
}

void phase_2(coord start, coord end)
{
	std::list<coord> path;
	std::list<coord>::iterator it = path.begin();
	int currentVal;						//value of current point
	coord currentCoord;					//the coord

	path.push_back(end);				//add the end point to the list
	currentCoord = end;					// end is current coord
	currentVal = grid[end.y][end.x];	//value of end is current value

	while (currentVal > 0)
	{
		if (grid[currentCoord.y + 1][currentCoord.x] == (currentVal - 1))	//check around current for next point
		{
			if ((currentCoord.y + 1) < HEIGHT)		//stay in bounds
			{
				currentCoord.y++;					//change current point to the new point
				path.push_back(currentCoord);		//add to list
			}
		}
		else if (grid[currentCoord.y - 1][currentCoord.x] == (currentVal - 1))
		{
			if ((currentCoord.y - 1) >= 0)
			{
				currentCoord.y--;
				path.push_back(currentCoord);
			}
		}
		else if (grid[currentCoord.y][currentCoord.x + 1] == (currentVal - 1))
		{
			if ((currentCoord.x + 1) < WIDTH)
			{
				currentCoord.x++;
				path.push_back(currentCoord);
			}
		}
		else if (grid[currentCoord.y][currentCoord.x - 1] == (currentVal - 1))
		{
			if ((currentCoord.x - 1) >= 0)
			{
				currentCoord.x--;
				path.push_back(currentCoord);
			}
		}
		currentVal--;		//set new value to search for;
	}
}