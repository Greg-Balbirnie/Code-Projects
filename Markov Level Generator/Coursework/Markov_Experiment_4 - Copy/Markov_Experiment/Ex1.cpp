#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "GdiplusHelperFunctions.h"
#include <chrono>

#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

#pragma comment (lib,"Gdiplus.lib")

const int MAX_WIDTH = 500;
const int MAX_HEIGHT = 500;

// Function prototypes
void Calculate_Probabilities();
void Generate_Level();
int Check_Tile(int start_x, int start_y);
void Display_Level(int[MAX_WIDTH][MAX_HEIGHT]);
void Draw_Bitmap();

int level[MAX_WIDTH][MAX_HEIGHT] = { 0 };
int new_level[MAX_WIDTH][MAX_HEIGHT] = { 0 };

// The number of unique tiles
const int MAX_TILES = 100;

int instances[MAX_TILES][MAX_TILES][MAX_TILES];	// The number of instances of a given state
float probability[MAX_TILES][MAX_TILES][MAX_TILES][MAX_TILES];	// The probability of state x, given state y on left, state z above and state w in top left

// The number of tiles in this image
int tile_num = 0;

const int MAX_TILE_SIZE = 64;	// The height and width, in pixels, of a tile

// The tile size of the entered level
int tile_size = 0;

// The size of the level in pixels
UINT pixel_width;
UINT pixel_height;

// The size of the level in tiles
int tile_width;
int tile_height;

struct Tile
{
	//Pixel pixel[16][16];
	Color pixel[MAX_TILE_SIZE][MAX_TILE_SIZE];
	int value;	// The value used in the algorithm
};

vector<Tile> known_tiles;

Bitmap* level_image;	// Image of the level
Bitmap* new_level_image;	// Image created of the new level

// Define the alias "the_clock" for the clock type we're going to use.
typedef std::chrono::steady_clock the_clock;

void main()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	bool valid = false;	// If the seklection is valid
	bool valid_answer = false;	// If the player's choice is valid

	// Loop until valid
	while (!valid)
	{
		// Let the user select the game
		cout << "What game would you like to use?" << endl;
		cout << "1 = Kid Icarus" << endl;
		cout << "2 = Super Mario Bros" << endl;
		cout << "3 = The Legend of Zelda" << endl;
		cout << "4 = Custom" << endl;
		int level_select;	// Number of level
		cin >> level_select;

		// Set up selected image
		switch (level_select)
		{
		case 1:
			level_image = new Bitmap(L"Levels/No_Pillars.png");
			tile_size = 16;
			valid = true;
			break;
		case 2:
			level_image = new Bitmap(L"Levels/Mario_Level.png");
			tile_size = 16;
			valid = true;
			break;
		case 3:
			level_image = new Bitmap(L"Levels/Zelda_Level.png");
			tile_size = 16;
			valid_answer = false;

			while (!valid_answer)
			{
				// Warn player about time
				cout << "This map is large and may take a long time. Continue? (y/n)" << endl;
				char answer;
				cin >> answer;

				// If they want to continue it is valid
				if (answer == 'y')
				{
					valid = true;
					valid_answer = true;
				}
				else if (answer == 'n')
				{
					valid_answer = true;
				}
			}
			break;
		case 4:
			level_image = new Bitmap(L"Levels/custom.png");

			// Get the tile size from the user
			cout << "What is the tile size of the game you are generating?" << endl;
			cin >> tile_size;

			valid = true;

			break;
		default:
			break;
		}
	}
	// Start timing
	the_clock::time_point start = the_clock::now();

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
		}

		// Show progress
		cout << "Checked row (" << j << " of " << tile_height << ")" << endl;
	}

	// Print the number of known tiles
	tile_num = known_tiles.size();
	cout << "There are " << tile_num << " unique tiles." << endl;

	// Check if there are too many tiles
	if (tile_num > 100)
	{
		cout << "The level generator will only work with up to 100 unique tiles." << endl;
		while (true)
		{
			// Hang
		}
	}

	// Show the level as numbers
	//Display_Level(level);

	// Seed randomiser
	srand(time(NULL));

	// New line
	cout << endl;

	Calculate_Probabilities();

	// New line
	cout << endl;

	Generate_Level();

	//Display_Level(new_level);

	Draw_Bitmap();

	// Stop timing
	the_clock::time_point end = the_clock::now();

	// Compute the difference between the two times in milliseconds
	auto time_taken = duration_cast<milliseconds>(end - start).count();
	cout << "Completed in " << time_taken << " ms." << endl;

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

				// A counter to loop through the possible tiles
				int k = 0;

				// Loop through the tiles
				while (k < possible_matches.size())
				{
					// Check all componants of colour
					// If they do not match
					if (pixel_colour->ToCOLORREF() != possible_matches[k]->pixel[i - start_x][j - start_y].ToCOLORREF())
					{
						// Remove from possible matches
						possible_matches.erase(possible_matches.begin() + k);
					}
					else
					{
						// Only increment k if none removed as if one is removed the next one will move down
						k++;
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

void Display_Level(int print_level[MAX_WIDTH][MAX_HEIGHT])
{
	// Loop through all ints 
	for (int y = 0; y < tile_height; y++)
	{
		for (int x = 0; x < tile_width; x++)
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

void Calculate_Probabilities()
{
	// Loop through all ints (except fist row/column as they have nothing above or left
	for (int y = 1; y < tile_height; y++)
	{
		// Show progress
		cout << "Calculated probabilities for row (" << y << " of " << tile_height << ")" << endl;

		for (int x = 1; x < tile_width; x++)
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
}

void Generate_Level()
{
	// Set level to original level
	for (int j = 0; j < tile_height; j++)
	{
		for (int i = 0; i < tile_width; i++)
		{
			new_level[i][j] = level[i][j];
		}
	}

	// Create pointers to height and width
	int* limit_first;
	int* point_first;

	int* limit_sec;
	int* point_sec;

	int x = 0;
	int y = 0;

	if (tile_width >= tile_height)
	{
		// Loop through height first
		limit_first = &tile_height;
		point_first = &y;

		// Loop through width second
		limit_sec = &tile_width;
		point_sec = &x;
	}
	else
	{
		// Loop through width first
		limit_first = &tile_width;
		point_first = &x;

		// Loop through height second
		limit_sec = &tile_height;
		point_sec = &y;
	}
	
	// A bool for if it found a correct tile
	bool is_accepted;

	// Initialise to 1
	*point_sec = 1;

	while (*point_sec < *limit_sec)
	{
		// Initialise to 1
		*point_first = 1;
		// Show progress
		cout << "Generating slice (" << *point_sec << " of " << *limit_sec << ")" << endl;

		while (*point_first < *limit_first)
		{
			// Set tile to unaccepted
			is_accepted = false;

			while (!is_accepted)
			{
				// Make sure x isn't out of range
				if (x < 1)
				{
					x = 1;
				}

				// Make sure y isn't out of range
				if (y < 1)
				{
					y = 1;
				}
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
					float rando = rand() % (int)test_total;

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

							// Accept the value
							is_accepted = true;

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
					// If y or x are 1 it will get stuck
					if (y == 1 || x == 1)
					{
						// Just set to 0
						new_level[x][y] = 0;

						// Accept the value
						is_accepted = true;
					}
					else
					{
						// Move back to top left tile to remove problem
						x -= 1;
						y -= 1;
					}
				}
			}
			(*point_first)++;
		}
		(*point_sec)++;
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