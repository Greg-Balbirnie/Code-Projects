////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include "Player.h"
#include <queue>

// The IP address for the server to listen on
sf::IpAddress SERVERIP = "127.0.0.1";

// Choose an arbitrary port for opening sockets
unsigned short port = 50001;

// Set up socket
sf::UdpSocket serverSocket;

// Set up message identifier
std::string send_identifier;
std::string receive_identifier;

// Set up packets
sf::Packet receive_packet;

// Define some constants
const float pi = 3.14159f;
const int gameWidth = 800;
const int gameHeight = 600;
sf::Vector2f characterSize(50, 50);
float ballRadius = 10.f;

// Create the players
Player player[2];

// Create the ball
sf::CircleShape ball;

// Create barrier
sf::RectangleShape barrier;

// Set play state
bool isPlaying;

// Function prototypes
void Send_Message();
void Start_Game(sf::Clock);
sf::Vector2f predict_position(int);

// Set up struct for queue
struct OppPosDetails
{
	float x;
	float y;
	float time;
};

// Set up queue of positions
std::deque<OppPosDetails> position_queue[2];

// Declare the predicted position
sf::Vector2f predicted_position[2];

// Set up timer
sf::Clock game_timer;
sf::Clock lerp_timer[2];

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Create clock
	sf::Clock clock;

	// Listen to messages on the specified port
	if (serverSocket.bind(port) != sf::Socket::Done)
	{
		return 0;
	}
	std::cout << "Server is listening to port " << port << ", waiting for a message... " << std::endl;

	// Set to non blocking mode
	serverSocket.setBlocking(false);

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "Collection Race server",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Set up player 1
	player[0].setSize(characterSize - sf::Vector2f(3, 3));
	player[0].setOutlineThickness(3);
	player[0].setOutlineColor(sf::Color::Black);
	player[0].setFillColor(sf::Color(sf::Color::Blue));
	player[0].setOrigin(characterSize / 2.f);
	player[0].SetIP("0.0.0");
	player[0].SetPort(0);
	player[0].SetScore(0);

	// Set up player 2
	player[1].setSize(characterSize - sf::Vector2f(3, 3));
	player[1].setOutlineThickness(3);
	player[1].setOutlineColor(sf::Color::Black);
	player[1].setFillColor(sf::Color(sf::Color::Red));
	player[1].setOrigin(characterSize / 2.f);
	player[1].SetIP("0.0.0");
	player[1].SetPort(0);
	player[1].SetScore(0);

	// Set up the ball
	ball.setRadius(ballRadius - 3);
	ball.setOutlineThickness(3);
	ball.setOutlineColor(sf::Color::Black);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ballRadius / 2, ballRadius / 2);

	// Set up barrier
	sf::Vector2f barrierSize(rand() % 100 + 50, rand() % 100 + 50);
	barrier.setSize(barrierSize);
	barrier.setOutlineThickness(3);
	barrier.setOutlineColor(sf::Color::Black);
	barrier.setFillColor(sf::Color(200, 200, 100));
	barrier.setOrigin(barrierSize / 2.f);

	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("28 Days Later.ttf"))
		return EXIT_FAILURE;

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Collection Race\n Server");

	// Initialize the UI
	sf::Text player1UI;
	player1UI.setFont(font);
	player1UI.setCharacterSize(75);
	player1UI.setPosition(10.0f, 10.0f);
	player1UI.setFillColor(sf::Color::Blue);

	sf::Text player2UI;
	player2UI.setFont(font);
	player2UI.setCharacterSize(75);
	player2UI.setPosition(gameWidth - 85.0f, 10.0f);
	player2UI.setFillColor(sf::Color::Red);

	// Define the paddles properties
	const float playerSpeed = 400.f;

	// If there has been a new position update
	bool new_pos_received[2] = { false, false };

	// Set play state to false
	isPlaying = false;

	while (window.isOpen())
	{
		// The address and port of the received message
		sf::IpAddress fromAddress;
		unsigned short fromPort;

		// If the receive function has finidhed receiving data
		bool finished = false;

		// If the function is not yet finished
		while (!finished)
		{
			// Receive any necessary data from clients 
			switch (serverSocket.receive(receive_packet, fromAddress, fromPort))
			{
			// If it was successful
			case sf::Socket::Done:

				// Take identifier from packet
				receive_packet >> receive_identifier;

				// Check the identifier
				if (receive_identifier == "lobby")	// If the player wants to start the game
				{
					// Check if there is a first player
					if (player[0].GetIP() == "0.0.0")
					{
						// Set up player one as the new player
						player[0].SetIP(fromAddress);
						player[0].SetPort(fromPort);
					}
					// Check if there is a second player
					else if (player[1].GetIP() == "0.0.0")
					{
						// Set up player two as the new player
						player[1].SetIP(fromAddress);
						player[1].SetPort(fromPort);
						// Start the game on both machines
						Start_Game(clock);
					}
				}
				else if (receive_identifier == "player_position")	// A player's position update
				{
					// Create new variables for writing 
					sf::Vector2f pos;
					float time_received;

					// Take new position from packet
					receive_packet >> pos.x >> pos.y >> time_received;

					// Loop for both players
					for (int i = 0; i < 2; i++)
					{
						// If the message was from player 1
						if (fromAddress == player[i].GetIP() && fromPort == player[i].GetPort())
						{
							// Set player's last time
							player[i].SetLastTime(time_received);

							// Add position to queue
							OppPosDetails new_pos;
							new_pos.x = pos.x;
							new_pos.y = pos.y;
							new_pos.time = time_received;
							position_queue[i].push_back(new_pos);

							// Check how many are in position queue
							while (position_queue[i].size() > 2)
							{
								// Take one off
								position_queue[i].pop_front();
							}

							// Set predicted position
							predicted_position[i] = predict_position(i);
							// Reset lerp timer
							lerp_timer[i].restart();

							// Remember there was a new update
							new_pos_received[i] = true;
						}
					}
				}
				// Clear identifier and packet
				receive_identifier.clear();
				receive_packet.clear();
				break;
			case sf::Socket::NotReady:
				// There is no more to receive
				finished = true;
				break;
			default:
				// Show someting went wrong
				printf("There was an error receiving\n");
				break;
			}
		}

		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				// Close the window
				window.close();
				break;
			}
		}

		// If the game is running
		if (isPlaying)
		{
			// Find deltatime
			float deltaTime = clock.restart().asSeconds();

			// Display the players' scores
			player1UI.setString(std::to_string(player[0].GetScore()));
			player2UI.setString(std::to_string(player[1].GetScore()));

			// Loop for both players
			for (int i = 0; i < 2; i++)
			{
				// Move the players
				sf::Vector2f first_pos = player[i].getPosition();
				sf::Vector2f second_pos = predicted_position[i];

				// Find vector between points
				sf::Vector2f direction = second_pos - first_pos;

				// Lerp between positions
				sf::Vector2f set_pos = (first_pos + (direction * lerp_timer[i].getElapsedTime().asSeconds()));

				// Place opponant at lerped position
				player[i].setPosition(set_pos);

				// Check the collisions between the ball and the characters
				if (ball.getPosition().x > player[i].getPosition().x - characterSize.x / 2 &&
					ball.getPosition().x < player[i].getPosition().x + characterSize.x / 2 &&
					ball.getPosition().y > player[i].getPosition().y - characterSize.y / 2 &&
					ball.getPosition().y < player[i].getPosition().y + characterSize.y / 2)
				{
					// If the position is not in the barrier
					bool accepted = false;
					// Loop until position is not in the barrier
					do
					{
						// Randomize location
						sf::Vector2f rando(rand() % 500, rand() % 500);
						// Check if location is invalid
						if (rando.x > barrier.getPosition().x - barrierSize.x / 2 &&
							rando.x < barrier.getPosition().x + barrierSize.x / 2 &&
							rando.y > barrier.getPosition().y - barrierSize.y / 2 &&
							rando.y < barrier.getPosition().y + barrierSize.y / 2)
						{
							// Do not accept the position
							accepted = false;
						}
						// The location is invalid
						else
						{
							// Add one to score of player
							player[i].SetScore(player[i].GetScore() + 1);

							// If their score is at ten
							if (player[i].GetScore() >= 10)
							{
								// Whether the player won
								bool player_wins;

								// Send a message to the clients
								for (int j = 0; j < 2; j++)
								{
									// Check if this player is the one who got ten
									if (j == i)
									{
										player_wins = true;
									}
									else
									{
										player_wins = false;
									}

									// Set the identifier
									send_identifier = "game_end";
									
									// Put it in packet with win state
									sf::Packet final_packet;
									final_packet << send_identifier
										<< player_wins;

									// Send win/lose notification
									switch (serverSocket.send(final_packet, player[j].GetIP(), player[j].GetPort()))
									{
									case sf::Socket::Error:
										// Show someting went wrong
										printf("There was an error sending final message\n");
										break;
									}

									// Clear
									send_identifier.clear();
								}

								//Show the game is over
								pauseMessage.setString("Game End");

								// End the play state
								isPlaying = false;

								// Forget the players
								player[i].SetIP("0.0.0");
								player[i].SetPort(0);
							}

							// Set ball to generated location
							ball.setPosition(rando);

							// Accept position
							accepted = true;
						}
					} while (!accepted);

					// When all is done, send to players
					// Declare variables for sending
					sf::Vector2f ball_pos = ball.getPosition();

					// Send to both players
					for (int i = 0; i < 2; i++)
					{
						// Set the identifier
						send_identifier = "ball_update";
						// Put all info into the packet
						sf::Packet ball_packet;
						ball_packet << send_identifier
							<< ball_pos.x << ball_pos.y
							<< player[0].GetScore() << player[1].GetScore();

						// Send ball position and scores
						switch (serverSocket.send(ball_packet, player[i].GetIP(), player[i].GetPort())) 
						{
						case sf::Socket::Error:
							// Show someting went wrong
							printf("There was an error sending the ball position message\n");
							break;
						}

						// Clear identifier
						send_identifier.clear();
					}
				}
			}

			
			// Send the other player's position to the players
			for (int i = 0; i < 2; i++)
			{
				// Check if there has been a position update
				if (new_pos_received[i])
				{
					// Set the identifier
					send_identifier = "opponent_position";

					// Put it in packet
					sf::Packet position_packet;

					// Find the other player to send their position
					if (i == 0)
					{
						position_packet << send_identifier << player[1].getPosition().x << player[1].getPosition().y << player[1].GetLastTime();
					}
					else
					{
						position_packet << send_identifier << player[0].getPosition().x << player[0].getPosition().y << player[0].GetLastTime();
					} 

					// Send start notification
					switch (serverSocket.send(position_packet, player[i].GetIP(), player[i].GetPort()))
					{
					case sf::Socket::Error:
						// Show someting went wrong
						printf("There was an error sending the opponant position\n");
						break;
					}

					// Clear
					send_identifier.clear();

					// Set new position to false
					new_pos_received[i] = false;
				}
			}
		}

		// Clear the window
		window.clear(sf::Color(50, 200, 50));

		// If game is running
		if (isPlaying)
		{
			// Draw the players
			window.draw(player[0]);
			window.draw(player[1]);
			// Draw the barrier and ball
			window.draw(barrier);
			window.draw(ball);

			// Draw the UI
			window.draw(player1UI);
			window.draw(player2UI);
		}
		else
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}

		// Display things on screen
		window.display();
	}
	
	// Return
	return EXIT_SUCCESS;
}

void Start_Game(sf::Clock clock)
{
	// If the game is not running
	if (!isPlaying)
	{
		// (Re)start the game
		isPlaying = true;
		clock.restart();

		// Reset the position of the players
		player[0].setPosition(10 + characterSize.x / 2, gameHeight / 2);
		player[1].setPosition(gameWidth - 10 - characterSize.x / 2, gameHeight / 2);

		// Reset the position of the barrier and ball
		ball.setPosition(gameWidth / 2, gameHeight / 2);
		barrier.setPosition(rand() % 500 + 50, rand() % 500 + 50);

		// Reset the score of the players
		player[0].SetScore(0);
		player[1].SetScore(0);
		
		// Send start notification to both players
		for (int i = 0; i < 2; i++)
		{
			// Set the identifier
			send_identifier = "start";
			
			// Declare variables for writing
			sf::Vector2f barrier_pos = barrier.getPosition();
			sf::Vector2f barrier_size = barrier.getSize();
			sf::Vector2f ball_pos = ball.getPosition();
		
			// Create packet to send
			sf::Packet Start_packet;

			// Put information in packet
			Start_packet << send_identifier << i
				<< barrier_pos.x << barrier_pos.y 
				<< barrier_size.x << barrier_size.y
				<< ball_pos.x << ball_pos.y;

			// Send start message to the clients
			switch (serverSocket.send(Start_packet, player[i].GetIP(), player[i].GetPort()))
			{
			case sf::Socket::Error:
				// Show someting went wrong
				printf("There was an error sending the start message\n");
				break;
			}

			// Clear identifier
			send_identifier.clear();
		}

		// Start the timer
		game_timer.restart();
	}
}

sf::Vector2f predict_position(int player_num)
{
	// The position the opponant is predicted to go to 
	sf::Vector2f predicted_position;

	// If there are enough positions and they both have different time stamps
	if (position_queue[player_num].size() >= 2 && (position_queue[player_num].at(1).time != position_queue[player_num].at(0).time))	// Quick fix, cannot find problem
	{
		// Linear model
		float velocityX = (position_queue[player_num].at(1).x - position_queue[player_num].at(0).x) / (position_queue[player_num].at(1).time - position_queue[player_num].at(0).time);
		float velocityY = (position_queue[player_num].at(1).y - position_queue[player_num].at(0).y) / (position_queue[player_num].at(1).time - position_queue[player_num].at(0).time);

		// Find predicted position
		predicted_position.x = (velocityX * (game_timer.getElapsedTime().asSeconds() - position_queue[player_num].at(1).time)) + position_queue[player_num].at(1).x;
		predicted_position.y = (velocityY * (game_timer.getElapsedTime().asSeconds() - position_queue[player_num].at(1).time)) + position_queue[player_num].at(1).y;

		// Return position
		return predicted_position;
	}
	// If there have not yet been enough times or they have the same time stamps
	else
	{
		// Set prediction to last known position
		predicted_position.x = position_queue[player_num].back().x;
		predicted_position.y = position_queue[player_num].back().y;

		// Return position
		return predicted_position;
	}
}

//The new position variable was being set to true whenever either player had a new position
//and so the positions and times were being sent twice for every new position message 
//instead of just once.
//This meant that every set of two positions received by the client were the same and had 
//the same time stamp and the predicted position was found by a formula that tried to divide 
//by zero.
//I made the new position boolean into an array od two (one for each client) which mostly 
//fixed the issue and made the interpolation much smoother.