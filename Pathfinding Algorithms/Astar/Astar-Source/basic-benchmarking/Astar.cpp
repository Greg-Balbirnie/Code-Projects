//includes
#include<chrono>
#include<thread>
#include<iostream>
#include<time.h> 
#include<set>
#include<stdlib.h>
#include<list>
#include<vector>
#include<math.h>

//namespace
using std::chrono::duration_cast;
using std::chrono::milliseconds;

using namespace std;

//the coordinate struct
struct node
{
	int x, y;
	int f, g, h;
	std::vector<node*> neighbours;
	bool block;
	node* parent;
};
bool operator<(const node& lhs, const node& rhs)
{
	return std::tie(lhs.x, lhs.y, lhs.g, lhs.h, lhs.f, lhs.parent, lhs.neighbours) < std::tie(rhs.x, rhs.y, rhs.g, rhs.h, rhs.f, rhs.parent, rhs.neighbours);
}

//function prototypes
void initialise(node*, node*);
float heuristic(node*, node*);
node* find_min(set<node*>);
void Astar(node*, node*);
void Trace(node*, node*);
bool isIn(set<node*>, node*);
//void print();

//Width and Height constants
#define WIDTH 20
#define HEIGHT 20

//define the grid
node grid[HEIGHT][WIDTH];

void main()
{
	//define start and end points as coordinates
	int randoX;
	int randoY;
	node* start;
	node* end;

	start = &grid[0][0];
	end = &grid[HEIGHT - 1][WIDTH - 1];

	initialise(start, end);			//initialise the grid
	Astar(start, end);				//start algorithm
	Trace(start, end);
}

void initialise(node* start, node* end)
{
	//set up randomiser
	srand(time(NULL));
	int rando;

	//loop through width and height
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			grid[i][j].x = i;
			grid[i][j].y = j;
			rando = rand() % 5 + 1;
			if (rando == 5)
			{
				grid[i][j].block = true;	//randomly set some to -2
			}
			else
			{
				grid[i][j].block = false;	//set the rest to -1
			}
		}
	}
	//make start and end not blocked
	start->block = false;
	end->block = false;
}

float heuristic(node* n, node* end)
{
	return (abs(n->x - end->x) + abs(n->y - end->y));
}

node* find_min(set<node*> N)
{
	std::set<node*>::iterator it;
	node* min;
	min = *N.begin();
	for (it = N.begin(); it != N.end(); it++)
	{
		if ((*it)->f < min->f)
		{
			min = *it;
		}
	}
	return min;
}

bool isIn(set<node*> X, node* Y)
{
	std::set<node*>::iterator it;
	for (it = X.begin(); it != X.end(); it++)
	{
		if ((*it)->x == Y->x && (*it)->y == Y->y)
		{
			return true;
		}
	}
	return false;
}

void Astar(node* start, node* end)
{
	std::set<node*> open;
	std::set<node*> closed;
	node* current;

	start->g = 0;
	start->f = start->g + heuristic(start, end);	//f = distance travelled + distance left
	open.insert(start);

	while (open.empty() == false)	//loop until no more in open
	{
		current = find_min(open);	//start at min f

		if (current->x == end->x && current->y == end->y)
		{
			break;					//stop if end is found
		}

		open.erase(current);
		closed.insert(current);

		//add neighbours
		
		if (current->y + 1 < WIDTH)
		{
			if (grid[current->x][current->y + 1].block == false)
			{
				current->neighbours.push_back(&grid[current->x][current->y + 1]);
			}
			if (current->x + 1 < HEIGHT)
			{
				if (grid[current->x + 1][current->y + 1].block == false)
				{
					current->neighbours.push_back(&grid[current->x + 1][current->y + 1]);
				}
			}
		}
		if (current->y - 1 >= 0)
		{
			if (grid[current->x][current->y - 1].block == false)
			{
				current->neighbours.push_back(&grid[current->x][current->y - 1]);
			}
			if (current->x - 1 >= 0)
			{
				if (grid[current->x - 1][current->y - 1].block == false)
				{
					current->neighbours.push_back(&grid[current->x - 1][current->y - 1]);
				}
			}
		}		
		if (current->x + 1 < HEIGHT)
		{
			if (grid[current->x + 1][current->y].block == false)
			{
				current->neighbours.push_back(&grid[current->x + 1][current->y]);
			}
			if (current->y - 1 >= 0)
			{
				if (grid[current->x + 1][current->y - 1].block == false)
				{
					current->neighbours.push_back(&grid[current->x + 1][current->y - 1]);
				}
			}
		}
		if (current->x - 1 >= 0)
		{
			if (grid[current->x - 1][current->y].block == false)
			{
				current->neighbours.push_back(&grid[current->x - 1][current->y]);
			}
			if (current->y + 1 < WIDTH)
			{
				if (grid[current->x - 1][current->y + 1].block == false)
				{
					current->neighbours.push_back(&grid[current->x - 1][current->y + 1]);
				}
			}
		}

		for (auto neighbour : current->neighbours)	//loop through neighbours
		{
			if (isIn(closed, neighbour) == false)
			{
				float new_g = current->g + 1;		//find next g val

				if (isIn(open, neighbour) == false)
				{
					//set values and add to open
					neighbour->g = new_g;
					neighbour->f = neighbour->g + heuristic(neighbour, end);
					neighbour->parent = current;
					open.insert(neighbour);
				}
				else if (new_g <= neighbour->g)
				{
					//update values
					neighbour->g = new_g;
					neighbour->f = neighbour->g + heuristic(neighbour, end);
					neighbour->parent = current;
				}
			}
		}
	}
}

void Trace(node* start, node* end)
{
	std::list<node*> path;
	node* current = end;
	while (current != start)	//loop back to start
	{
		path.push_back(current);
		current = current->parent;
	}
	path.push_back(start);
	path.reverse();				//flip the path 
}

