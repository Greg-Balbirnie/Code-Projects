#include "Creature.h"

sf::Creature::Creature()
{
	setRadius(10.0f);
	Set_Food(0);
	alive = true;

	// Set to random position
	setPosition(rand() % 500, rand() % 500);
	Set_Centre();
}

sf::Creature::Creature(float init_radius)
{
	setRadius(init_radius);
	Set_Food(0);
	alive = true;

	// Set to random position
	setPosition(rand() % 500, rand() % 500);
	Set_Centre();
}

sf::Creature::~Creature()
{
}

sf::Vector2f sf::Creature::Get_Centre()
{
	return centre;
}

void sf::Creature::Set_Centre()
{
	centre = getPosition() + Vector2f(getRadius(), getRadius());
}

float sf::Creature::Get_Food()
{
	return food_collected;
}

void sf::Creature::Set_Food(float new_food)
{
	food_collected = new_food;
}

Chromosome* sf::Creature::Get_Speed_Chromosome()
{
	return &speed;
}

void sf::Creature::Set_Speed_Chromosome(Chromosome new_speed)
{
	speed.Set_To_Chromosome(new_speed);
}

int sf::Creature::Get_Speed()
{
	return speed.Get_Fitness();
}

void sf::Creature::Set_Speed(int)
{
}

sf::Vector2f sf::Creature::Get_Direction()
{
	return direction;
}

void sf::Creature::Set_Direction(Vector2f new_direction)
{
	direction = new_direction;
}

void sf::Creature::Add_Food(float amount)
{
	food_collected += amount;
}

void sf::Creature::Reset_Food()
{
	food_collected = 0.0f;
}

bool sf::Creature::Is_Alive()
{
	return alive;
}

void sf::Creature::Kill()
{
	alive = false;
}
