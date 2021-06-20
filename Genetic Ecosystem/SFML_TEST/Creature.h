#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include "Chromosome.h"
#include <SFML/Graphics.hpp>

namespace sf 
{
	class Creature : public CircleShape
	{
	public:
		Creature();
		Creature(float radius);
		~Creature();

		Vector2f Get_Centre();
		void Set_Centre();

		float Get_Food();
		void Set_Food(float food_amount);

		Chromosome* Get_Speed_Chromosome();
		void Set_Speed_Chromosome(Chromosome new_chromosome);

		int Get_Speed();
		void Set_Speed(int new_speed);

		Vector2f Get_Direction();
		void Set_Direction(Vector2f new_direction);

		void Add_Food(float food_amount);
		void Reset_Food();

		bool Is_Alive();
		void Kill();

	private:
		Vector2f centre;	// The centre of the sprite
		float food_collected;	// The amount of food the creature has created
		Chromosome speed;	// The chromosome determining the creature's speed
		Vector2f direction;	// The direction to the nearest food source
		bool alive;
	};
}
