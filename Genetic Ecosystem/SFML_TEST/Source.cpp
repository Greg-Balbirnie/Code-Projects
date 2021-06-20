#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "Collision.h"
#include <iostream>
#include "Genetic.h"
#include <SFML/System/Vector2.hpp>
#include "Creature.h"
#include <ctime>

// Function Prototypes
void init();
void update();
void render();
void render_debug_text();
void finished();

// Variables
sf::RenderWindow window(sf::VideoMode(1000, 700), "The Genetic Ecosystem");
sf::CircleShape food(50.0f);

Collision collision_manager;
Genetic genetic_manager;

sf::Font main_font;

clock_t start;
double duration;

clock_t death_clock;
double lifetime;

float speed_modifier;

void main()
{
	// Initialise variables
	init();

	// Start timers
	start = clock();
	death_clock = clock();

	// Loop until window is closed
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		// Call update
		update();

		// Render images
		//render();

		// Set duration
		lifetime = (clock() - death_clock) / (double)CLOCKS_PER_SEC;

		// Check duration
		if (lifetime >= 15 / speed_modifier)
		{
			// Create children
			genetic_manager.Evaluate_Fitness();

			// Loop through all creatures
			for (int i = 0; i < genetic_manager.population.size(); i++)
			{
				// Create creature pointer
				sf::Creature* this_c = &genetic_manager.population[i];

				// Check creature is alive
				if (this_c->Is_Alive())
				{
					// Reset food
					this_c->Reset_Food();

					// Check for max speed
					if (this_c->Get_Speed_Chromosome()->Get_Fitness() == 16)
					{
						// Finish the algorithm
						finished();
					}
				}
				// Delete pointer TODO
				//delete(&this_c);
			}

			// Reset time
			death_clock = clock();
		}
	}
}

void init()
{
	// Generate the creatures
	genetic_manager.Generate_Population();

	// Set the colour of the food
	food.setFillColor(sf::Color::Red);
	food.setPosition(200, 200);

	// Text
	main_font.loadFromFile("../Font/Lato-Regular.ttf");

	// Set speed modifier
	speed_modifier = 5.0f;
}

void update()
{
	//Close the app
	if (GetKeyState(VK_ESCAPE) & 0x0800)
	{
		window.close();
	}

	// Loop through all creatures
	for (int i = 0; i < genetic_manager.population.size(); i++)
	{
		// Create creature pointer
		sf::Creature* this_c = &genetic_manager.population[i];

		// Check creature is alive
		if (this_c->Is_Alive())
		{
			//Find closest food source TODO

			// Find direction to food source
			sf::Vector2f dir = food.getPosition() + sf::Vector2f(food.getRadius(), food.getRadius()) - this_c->Get_Centre();

			// Find magnitude of direction
			float dist = sqrt(pow(dir.x, 2) + pow(dir.y, 2));

			if (dist != 0)
			{
				// Divide dir by dist to get unit vector
				dir /= dist;
			}
			else
			{
				// Set dir to 0
				dir = sf::Vector2f(0.0f, 0.0f);
			}

			// Set creature direction to food source
			this_c->Set_Direction(dir);

			// Move creature
			this_c->move(this_c->Get_Speed() * (speed_modifier / 100) * dir.x, this_c->Get_Speed() * (speed_modifier / 100) * dir.y);
			this_c->Set_Centre();

			// Check if collided
			if (collision_manager.collide(*this_c, food))
			{
				// Make food pile smaller
				food.setRadius(food.getRadius() - 1 * (speed_modifier / 100));
				food.move(sf::Vector2f(speed_modifier / 100, speed_modifier / 100));

				// If food is depleted
				if (food.getRadius() <= 0)
				{
					// remove food pile
					// TODO
					food.setRadius(50.0f);
					food.setPosition(rand() % 500, rand() % 350);
				}

				// Add food to creature
				this_c->Add_Food(0.01);
			}
		}
		// Delete pointer TODO
		//delete(&this_c);
	}
}

void render()
{
	// Clear the previous frame
	window.clear();

	// render the circle
	//window.draw(test_creature);
	window.draw(food);

	// Create text for the food gathered
	sf::Text display_text;

	// Set up display text
	display_text.setFont(main_font);
	display_text.setFillColor(sf::Color::White);
	display_text.setCharacterSize(15);

	for (int i = 0; i < genetic_manager.population.size(); i++)
	{
		// Create creature pointer
		sf::Creature* this_c = &genetic_manager.population[i];

		// Check creature is alive
		if (this_c->Is_Alive())
		{
			// Draw Creature TODO
			window.draw(*this_c);

			// Show food text
			display_text.setString("Food: " + to_string((int)this_c->Get_Food()));
			display_text.setPosition(this_c->getPosition() + sf::Vector2f(0.0f, -35.0f));
			window.draw(display_text);

			// Show speed text
			display_text.setString("Speed: " + to_string((int)this_c->Get_Speed_Chromosome()->Get_Fitness()));
			display_text.setPosition(this_c->getPosition() + sf::Vector2f(0.0f, -20.0f));
			window.draw(display_text);
		}

		// Delete pointer TODO
		//delete(&this_c);
	}

	//render_debug_text();

	// Show the window
	window.display();
}

void render_debug_text()
{
	// Create text for the food gathered
	sf::Text debug_text;

	// Set display text
	debug_text.setFont(main_font);
	debug_text.setFillColor(sf::Color::White);

	for (int i = 0; i < genetic_manager.population.size(); i++)
	{
		debug_text.setString(to_string(genetic_manager.population[i].getPosition().x));
		debug_text.setPosition(sf::Vector2f(0.0f, (float)i * 25));

		window.draw(debug_text);
	}
}

void finished()
{
	cout << "Done in " <<  (clock() - start) / 1000.0f << " seconds" << endl;
	cout << "Generation: " << genetic_manager.population.back().Get_Speed_Chromosome()->Get_Generation();
	while (true)
	{
		// Hang
	}
}
