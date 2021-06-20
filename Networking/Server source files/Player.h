#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class Player : public sf::RectangleShape
{
private:
	sf::IpAddress address;	// The IP address the player is at
	unsigned short port;	// The port the player is at
	int score;	//the player's score
	float last_time;	//the last time it sent a message
public:
	Player();
	~Player();
	// Getters
	sf::IpAddress Player::GetIP();
	unsigned short Player::GetPort();
	int Player::GetScore();
	float Player::GetLastTime();
	// Setters
	void Player::SetIP(sf::IpAddress);
	void Player::SetPort(unsigned short);
	void Player::SetScore(int);
	void Player::SetLastTime(float);
};

