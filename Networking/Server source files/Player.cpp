#include "Player.h"



Player::Player()
{
}


Player::~Player()
{
}

sf::IpAddress Player::GetIP()
{
	return address;
}

unsigned short Player::GetPort()
{
	return port;
}

int Player::GetScore()
{
	return score;
}

float Player::GetLastTime()
{
	return last_time;
}

void Player::SetIP(sf::IpAddress new_ip)
{
	address = new_ip;
}

void Player::SetPort(unsigned short new_port)
{
	port = new_port;
}

void Player::SetScore(int new_score)
{
	score = new_score;
}

void Player::SetLastTime(float new_time)
{
	last_time = new_time;
}
