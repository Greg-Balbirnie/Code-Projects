#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "GdiplusHelperFunctions.h"

#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;

#pragma comment (lib,"Gdiplus.lib")

// TODO This is the amount of tiles in kid icarus but this wont work with other maps
const int WIDTH = 16;
const int HEIGHT = 180;

// Function prototypes
void Generate_Random_Level();
void Calculate_Probabilities();
void Generate_Level();
int Check_Tile(int start_x, int start_y);
void Display_Level(int[WIDTH][HEIGHT]);
void Draw_Bitmap();

int level[WIDTH][HEIGHT] = { 0 };
int new_level[WIDTH][HEIGHT] = { 0 };

// TODO This will change game to game
const int tile_num = 24;

int instances[tile_num][tile_num][tile_num];	// The number of instances of a given state
float probability[tile_num][tile_num][tile_num][tile_num];	// The probability of state x, given state y on left, state z above and state w in top left

// TODO Let the user input this value
const int tile_size = 16;	// The height and width, in pixels, of a tile

// The size of the level in pixels
UINT pixel_width;
UINT pixel_height;

// The size of the level in tiles
int tile_width;
int tile_height;

/*struct Pixel
{
	int red;
	int green;
	int blue;
};*/

struct Tile
{
	//Pixel pixel[16][16];
	Color pixel[tile_size][tile_size];
	int value;	// The value used in the algorithm
};

vector<Tile> known_tiles;

Bitmap* level_image;	// Image of the level
Bitmap* new_level_image;	// Image created of the new level



void Generate()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Load in the image of the level
	//level_image = new Bitmap(L"KidIcarusLevel1-1BG.png");
	level_image = new Bitmap(L"../res/No_Pillars.png");

	// Find the width and height
	pixel_width = level_image->GetWidth();	// The width of the image in pixels
	pixel_height = level_image->GetHeight();	// The height of the image in pixels

	tile_width = pixel_width / tile_size;	// The width of the image in tiles
	tile_height = pixel_height / tile_size;	// The height of the image in tiles

	// Check every tile
	for (int j = 0; j < tile_height; j++)
	{
		for (int i = 0; i < tile_width; i++)
		{
			// Assign tile value to grid
			level[i][j] = Check_Tile(i * tile_size, j * tile_size);

			// Show progress
			//cout << "Checked tile (" << i << "," << j << ")" << endl;			
		}
	}

	// Print the number of known tiles
	cout << "There are " << known_tiles.size() << " different tiles." << endl;

	// Show the level as numbers
	//Display_Level(level);

	// Take random seed
	cout << "Please give a seed for the randomiser" << endl;
	int seed = 5;
	cin >> seed;
	cout << endl;

	// Seed randomiser
	srand(seed);

	//Generate_Random_Level();

	// New line
	cout << endl;

	Calculate_Probabilities();

	// New line
	cout << endl;

	Generate_Level();

	Display_Level(new_level);

	Draw_Bitmap();

	while (true)
	{
		// Hang
	}

	// deallocate space for images
	delete level_image;
	delete new_level_image;

	Gdiplus::GdiplusShutdown(gdiplusToken);
}

// Find what the tile is and return it's value
int Check_Tile(int start_x, int start_y)
{
	// Check if there are existing tiles
	if (known_tiles.size() > 0)
	{
		// At this point any tile could match the new one
		// A pointer to every tile that is a possible match
		vector<Tile*> possible_matches;

		// Loop through known tiles and push to possible matches
		for (int i = 0; i < known_tiles.size(); i ++)
		{
			// Add memory location of tile
			possible_matches.push_back(&known_tiles.at(i));
		}

		// Create new tile
		Tile* new_tile = new Tile;

		// loop through the pixels that make up the tile
		for (int j = start_y; j < start_y + tile_size; j++)
		{
			for (int i = start_x; i < start_x + tile_size; i++)
			{
				Color* pixel_colour = new Color;
				level_image->GetPixel(i, j, pixel_colour);
				// i - start_x will always start on 0 and go to 15
				new_tile->pixel[i - start_x][j - start_y] = *pixel_colour;

				for (int k = 0; k < possible_matches.size(); k++)
				{
					// Check all componants of colour
					// If they do not match
					if (new_tile->pixel[i - start_x][j - start_y].GetRed() != possible_matches.at(k)->pixel[i - start_x][j - start_y].GetRed()
						|| new_tile->pixel[i - start_x][j - start_y].GetGreen() != possible_matches.at(k)->pixel[i - start_x][j - start_y].GetGreen()
						|| new_tile->pixel[i - start_x][j - start_y].GetBlue() != possible_matches.at(k)->pixel[i - start_x][j - start_y].GetBlue())
					{
						// Remove from possible matches
						possible_matches.erase(possible_matches.begin() + k);
					}
				}
			}
		}

		// Check if it matches an existing tile or is new
		if (possible_matches.size() == 0)
		{
			// Give a value to the tile
			new_tile->value = known_tiles.size();

			// Add to the list of known tiles
			known_tiles.push_back(*new_tile);

			// Add value to grid
			return new_tile->value;

			// Delete pointer
			delete new_tile;
		}
		else if (possible_matches.size() == 1)
		{
			// Give the value of the matched tile
			auto it = possible_matches.begin();
			new_tile->value = (*it)->value;

			// Add value to grid
			return new_tile->value;

			// Delete pointer
			delete new_tile;
		}
		else
		{
			// Show how many possible matches there are
			cout << "There are " << possible_matches.size() << " possible matches. An error has occurred." << endl;
			while (true)
			{
				// Hang
			}
		}

		// Empty possible matches
		possible_matches.clear();
	}
	else
	{
		// Create new tile
		Tile* new_tile = new Tile;

		// loop through the pixels that make up the tile
		for (int j = start_y; j < start_y + tile_size; j++)
		{
			for (int i = start_x; i < start_x + tile_size; i++)
			{
				Color* pixel_colour = new Color;
				level_image->GetPixel(i, j, pixel_colour);
				// i - start_x will always start on 0 and go to 15
				new_tile->pixel[i - start_x][j - start_y] = *pixel_colour;
				// Deallocate memory
				delete pixel_colour;
			}
		}

		// Give a value to the tile
		new_tile->value = known_tiles.size();

		// Add to the list of known tiles
		known_tiles.push_back(*new_tile);

		// Add value to grid
		return new_tile->value;

		// delete pointer
		delete new_tile;
	}
}

void Display_Level(int print_level[WIDTH][HEIGHT] )
{
	// Loop through all ints 
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// Only 16 unique colours
			if (print_level[x][y] < 16)
			{
				// Print element
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), print_level[x][y] * 16);
				cout << " ";
			}
			else
			{
				// Print element
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (print_level[x][y] * 16) + 15);
				cout << "X";
			}
			Sleep(0);
		}
		// New line
		cout << endl;
	}
}

void Generate_Random_Level()
{
	// Loop through all ints 
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// randomly create 1's
			if ((rand() % 100) > 90)
			{
				level[x][y] = 1;
			}

			// Print element
			cout << level[x][y];
		}
		// New line
		cout << endl;
	}
}

void Calculate_Probabilities()
{
	// Loop through all ints (except fist row/column as they have nothing above or left
	for (int y = 1; y < HEIGHT; y++)
	{
		for (int x = 1; x < WIDTH; x++)
		{
			// Add to the probability that this tile is the state it is given that the tile to it's left, up and top left are the states they are
			probability[level[x][y]][level[x - 1][y]][level[x][y - 1]][level[x - 1][y - 1]]++;

			// Add one to instances of this state appearing
			instances[level[x - 1][y]][level[x][y - 1]][level[x - 1][y - 1]]++;
		}
	}

	// Change to percentage
	for (int w = 0; w < tile_num; w++)
	{
		for (int z = 0; z < tile_num; z++)
		{
			for (int y = 0; y < tile_num; y++)
			{
				for (int x = 0; x < tile_num; x++)
				{
					// If there are no instances
					if (instances[y][z][w] == 0)
					{
						// no chance of this occuring
						probability[x][y][z][w] = 0.0f;
					}
					// If there are any instances
					else
					{
						// Calculate the probability
						probability[x][y][z][w] /= instances[y][z][w];
						probability[x][y][z][w] *= 100.0f;
					}
				}
			}
		}
	}

	// Loop through all probabilities
	/*for (int w = 0; w < tile_num; w++)
	{
		for (int z = 0; z < tile_num; z++)
		{
			for (int y = 0; y < tile_num; y++)
			{
				for (int x = 0; x < tile_num; x++)
				{
					// Show the probability
					cout << x << y << z << w << ": " << probability[x][y][z][w] << "%" << endl;
				}
				// Take a new line
				cout << endl;
			}
		}
	}*/
}

void Generate_Level()
{
	// Loop through all ints 
	for (int y = 1; y < HEIGHT; y++)
	{
		for (int x = 1; x < WIDTH; x++)
		{
			//cout << "checking tile (" << x << "," << y << ")" << endl;

			// Create a vector of probabilities
			vector<float> probs;

			// Create test total
			float test_total = 0;

			// Loop through possible tiles
			for (int i = 0; i < tile_num; i++)
			{
				// Add probabilty of it being this tile
				probs.push_back(probability[i][new_level[x - 1][y]][new_level[x][y - 1]][new_level[x - 1][y - 1]]);

				// Add to test total
				test_total += probability[i][new_level[x - 1][y]][new_level[x][y - 1]][new_level[x - 1][y - 1]];
			}

			// Check total is > 0
			if (test_total > 0)
			{
				// Create a random number from 0 to 99
				float rando = rand() % 100;

				// create total probability and initialise to first probability
				float total_prob = probs[0];

				// Create probability pointer
				int prob_pointer = 0;

				// Loop
				while (prob_pointer < tile_num)
				{
					// Check random number
					if (rando <= total_prob)
					{
						// Set value to probability pointer
						new_level[x][y] = prob_pointer;

						// Break from while
						break;
					}
					else
					{
						// Increment probability pointer
						prob_pointer++;

						// Add to total probability
						total_prob += probs[prob_pointer];
					}
				}
			}
			else
			{
				// TODO Make this figure out what to do in the event of an impossible scenario
				//cout << "impossible" << endl;
				//new_level[x][y] = 0;

				//cout << "Impossibe tile (" << x << "," << y << ")" << endl;

				//TEST1 move back to top left tile to remove problem
				x -= 2; 
				y -= 1;
			}
		}
	}
}

void Draw_Bitmap()
{
	new_level_image = new Bitmap(pixel_width, pixel_height, PixelFormat32bppARGB);
	
	Color* colour = new Color(255, 0, 0);
	
	// Loop through all tiles
	for (int j = 0; j < tile_height; j++)
	{
		for (int i = 0; i < tile_width; i++)
		{
			// Find the value of this tile
			int val = new_level[i][j];

			// Loop through all pixels in the tiles
			for (int y = 0; y < tile_size; y++)
			{
				for (int x = 0; x < tile_size; x++)
				{
					// Known tiles are in order and contain a grid of pixels
					// Create a colour reference to this pixels colour
					COLORREF cr = known_tiles[val].pixel[x][y].ToCOLORREF();

					// Set colour to this pixel colour
					colour->SetFromCOLORREF(cr);

					// Set the pixel on the map
					new_level_image->SetPixel((i * tile_size) + x, (j * tile_size) + y, *colour);
				}
			}
		}
	}

	// The class id for png
    CLSID pngClsid;

	// Get the class id
	GetEncoderClsid(L"image/png", &pngClsid);

	// Save the image as png
	new_level_image->Save(L"generated_level.png", &pngClsid, NULL);
}