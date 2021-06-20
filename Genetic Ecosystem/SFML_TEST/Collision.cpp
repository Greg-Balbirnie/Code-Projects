#include "Collision.h"

Collision::Collision()
{
}


Collision::~Collision()
{
}

bool Collision::collide(sf::CircleShape c1, sf::CircleShape c2)
{
	float distance;
	float radii;
	
	sf::Vector2f* centre[2];

	// Find circle centres
	centre[0] = new sf::Vector2f(c1.getPosition().x + (c1.getRadius()), c1.getPosition().y + (c1.getRadius()));
	centre[1] = new sf::Vector2f(c2.getPosition().x + (c2.getRadius()), c2.getPosition().y + (c2.getRadius()));

	distance = sqrt(pow(centre[1]->x - centre[0]->x, 2) + pow(centre[1]->y - centre[0]->y, 2));
	radii = c1.getRadius() + c2.getRadius();

	if (distance <= radii)
	{
		return true;
	}
	else
	{
		return false;
	}
}
