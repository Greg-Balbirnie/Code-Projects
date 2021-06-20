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
#include <queue>

// Function prototypes
sf::Vector2f predict_position();

// The IP address for the server to listen on
sf::IpAddress SERVERIP = "127.0.0.1";

// Choose an arbitrary port for opening sockets
unsigned short port = 50001;

// Set up socket
sf::UdpSocket socket;

// Set up message identifier
std::string send_identifier;
std::string receive_identifier;

// Set up timer
sf::Clock game_timer;
sf::Clock send_timer;
sf::Clock lerp_timer;

// Frame Counter
int count = 0;

// Set up struct for queue
struct OppPosDetails
{
	float x;
	float y;
	float time;
};

// Set up queue of positions
std::deque<OppPosDetails> position_queue;

// Declare the predicted position
sf::Vector2f predicted_position;

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
	sf::Vector2f characterSize(50, 50);
	float ballRadius = 10.f;
	
	socket.setBlocking(false);

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "Collection Race",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Score variables
	int player1Score = 0;
	int player2Score = 0;

	// Create the first character
	sf::RectangleShape playerCharacter;
	playerCharacter.setSize(characterSize - sf::Vector2f(3, 3));
	playerCharacter.setOutlineThickness(3);
	playerCharacter.setOutlineColor(sf::Color::Black);
	playerCharacter.setOrigin(characterSize / 2.f);

	// Create the second character
	sf::RectangleShape opponentCharacter;
	opponentCharacter.setSize(characterSize - sf::Vector2f(3, 3));
	opponentCharacter.setOutlineThickness(3);
	opponentCharacter.setOutlineColor(sf::Color::Black);
	opponentCharacter.setOrigin(characterSize / 2.f);

	// Create the ball
	sf::CircleShape ball;
	ball.setRadius(ballRadius - 3);
	ball.setOutlineThickness(3);
	ball.setOutlineColor(sf::Color::Black);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ballRadius / 2, ballRadius / 2);

	// Create barrier
	sf::RectangleShape barrier;
	barrier.setOutlineThickness(3);
	barrier.setOutlineColor(sf::Color::Black);
	barrier.setFillColor(sf::Color(200, 200, 100));

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
	pauseMessage.setString("Collection Race\nPress space when ready");

	// Initialize the pause message
	sf::Text lobbyMessage;
	lobbyMessage.setFont(font);
	lobbyMessage.setCharacterSize(40);
	lobbyMessage.setPosition(170.f, 150.f);
	lobbyMessage.setFillColor(sf::Color::White);
	lobbyMessage.setString("Waiting for other player");

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

	// Clock to find deltatime
	sf::Clock clock;

	// The state of the game
	enum Game_State
	{
		INACTIVE,	// Before anything has happened
		LOBBY,		// When the player is wanting to start
		PLAYING		// When the game is running
	};

	// The current state of the game
	Game_State current_state = INACTIVE;

	while (window.isOpen())
	{
		// The address and port the messages came from
		sf::IpAddress fromAddress;
		unsigned short fromPort;

		// If all the messages have been received
		bool finished = false;

		// Set up receive packet
		sf::Packet receive_packet;

		while (!finished && current_state != INACTIVE)
		{
			// Recieve any necessary data from the server
			switch (socket.receive(receive_packet, fromAddress, fromPort))
			{
			//If it was successful
			case sf::Socket::Done:
				// What kind of message it was
				receive_packet >> receive_identifier;

				//check the identifier
				if (receive_identifier == "start")	// The game starting
				{
					// Declare received variables
					int player_num;
					sf::Vector2f barrier_pos;
					sf::Vector2f barrier_size;
					sf::Vector2f ball_pos;

					// Unpack data
					receive_packet >> player_num 
						>> barrier_pos.x >> barrier_pos.y 
						>> barrier_size.x >> barrier_size.y
						>> ball_pos.x >> ball_pos.y;
					
					// Check player num 
					if (player_num == 0)
					{
						// Set position and colour
						playerCharacter.setPosition(10 + characterSize.x / 2, gameHeight / 2);
						playerCharacter.setFillColor(sf::Color::Blue);
						opponentCharacter.setFillColor(sf::Color::Red);
					}
					else if (player_num == 1)
					{
						// Set position and colour
						playerCharacter.setPosition(gameWidth - 30 + characterSize.x / 2, gameHeight / 2);
						playerCharacter.setFillColor(sf::Color::Red);
						opponentCharacter.setFillColor(sf::Color::Blue);
					}

					// Set ball and Barrier position
					barrier.setPosition(barrier_pos);
					barrier.setSize(barrier_size);
					barrier.setOrigin(barrier_size / 2.f);
					ball.setPosition(ball_pos);

					// Set state to playing
					current_state = PLAYING;

					// Restart timer
					game_timer.restart();
				}
				else if (receive_identifier == "opponent_position")	// The position of the opponant
				{
					// Define variables for writing
					sf::Vector2f pos;
					float time_received;

					// Unpack variables from packets
					receive_packet >> pos.x >> pos.y >> time_received;

					// Add position and time to queue
					OppPosDetails new_pos;
					new_pos.x = pos.x;
					new_pos.y = pos.y;
					new_pos.time = time_received;
					position_queue.push_back(new_pos);

					// Check how many are in position queue
					while (position_queue.size() > 2)
					{
						// Take one off
						position_queue.pop_front();
					}

					// Set predicted position
					predicted_position = predict_position();
					
					// Reset lerp timer
					lerp_timer.restart();
				}
				else if (receive_identifier == "ball_update")	// The new position of the ball
				{
					// Create new variables for writing
					sf::Vector2f ball_pos;
					int new_score_1;
					int new_score_2;

					// Unpack variables from packet
					receive_packet >> ball_pos.x >> ball_pos.y
						>> new_score_1 >> new_score_2;

					// Set ball position
					ball.setPosition(ball_pos);

					// Set player scores
					player1Score = new_score_1;
					player2Score = new_score_2;
				}
				else if (receive_identifier == "game_end")	// The end of the game
				{
					// Create new variables for writing
					bool player_wins;

					// Take the boolean out of the packet
					receive_packet >> player_wins;

					// Check if the payer wins or loses
					if (player_wins)
					{
						// Show the player they win
						pauseMessage.setString("you win");
					}
					else
					{
						// Show the player they lose
						pauseMessage.setString("you lose");
					}

					// Display the pause screen again
					current_state = INACTIVE;
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

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				// If the game is currenly not started
				if (current_state == INACTIVE)
				{
					// (Re)start the game
					current_state = LOBBY;
					clock.restart();

					// Create a packet
					sf::Packet lobby_packet; 

					// Add information to packet
					send_identifier = "lobby";
					lobby_packet << send_identifier;

					// Send initial message to server
					switch (socket.send(lobby_packet, SERVERIP, port))
					{
					case sf::Socket::Error:
						// Show someting went wrong
						printf("There was an error sending the lobby message\n");
						break;
					}

					// Clear the identifier
					send_identifier.clear();
				}
			}
		}

		// If the game is playing
		if (current_state == PLAYING)
		{
			// Check if time is right
			if (send_timer.getElapsedTime().asSeconds() >= 0.1f)
			{
				// Reset timer
				send_timer.restart();

				// Set up packet
				sf::Packet pos_packet;

				// Set up packet information
				send_identifier = "player_position";
				sf::Vector2f pos = playerCharacter.getPosition();

				// Add information
				pos_packet << send_identifier << pos.x << pos.y << game_timer.getElapsedTime().asSeconds();

				// Send player position to the server
				switch (socket.send(pos_packet, SERVERIP, port))
				{
				case sf::Socket::Error:
					// Show someting went wrong
					printf("There was an error sending the player's position\n");
					break;
				}

				// Clear identifier
				send_identifier.clear();
			}

			// Find deltatime
			float deltaTime = clock.restart().asSeconds();

			// Display the players' scores
			player1UI.setString(std::to_string(player1Score));
			player2UI.setString(std::to_string(player2Score));

			// Take down key press
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
				(playerCharacter.getPosition().y - characterSize.y / 2 > 5.f))
			{
				// Move the player up
				playerCharacter.move(0.f, -playerSpeed * deltaTime);

				// Check if in barrier
				if (playerCharacter.getPosition().x + characterSize.x / 2 > barrier.getPosition().x - barrier.getSize().x / 2 &&
					playerCharacter.getPosition().x - characterSize.x / 2 < barrier.getPosition().x + barrier.getSize().x / 2 &&
					playerCharacter.getPosition().y + characterSize.y / 2 > barrier.getPosition().y - barrier.getSize().y / 2 &&
					playerCharacter.getPosition().y - characterSize.y / 2 < barrier.getPosition().y + barrier.getSize().y / 2)
				{
					// Move them back
					playerCharacter.move(0.f, playerSpeed * deltaTime);
				}
			}
			// Take down key press
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
				(playerCharacter.getPosition().y + characterSize.y / 2 < gameHeight - 5.f))
			{
				// Move the player down
				playerCharacter.move(0.f, playerSpeed * deltaTime);

				// Check if in barrier
				if (playerCharacter.getPosition().x + characterSize.x / 2 > barrier.getPosition().x - barrier.getSize().x / 2 &&
					playerCharacter.getPosition().x - characterSize.x / 2 < barrier.getPosition().x + barrier.getSize().x / 2 &&
					playerCharacter.getPosition().y + characterSize.y / 2 > barrier.getPosition().y - barrier.getSize().y / 2 &&
					playerCharacter.getPosition().y - characterSize.y / 2 < barrier.getPosition().y + barrier.getSize().y / 2)
				{
					// Move them back
					playerCharacter.move(0.f, -playerSpeed * deltaTime);
				}
			}
			// Take left key press
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
				(playerCharacter.getPosition().y - characterSize.y / 2 > 5.f))
			{
				// Move the player left
				playerCharacter.move(-playerSpeed * deltaTime, 0.f);

				// Check if in barrier
				if (playerCharacter.getPosition().x + characterSize.x / 2 > barrier.getPosition().x - barrier.getSize().x / 2 &&
					playerCharacter.getPosition().x - characterSize.x / 2 < barrier.getPosition().x + barrier.getSize().x / 2 &&
					playerCharacter.getPosition().y + characterSize.y / 2 > barrier.getPosition().y - barrier.getSize().y / 2 &&
					playerCharacter.getPosition().y - characterSize.y / 2 < barrier.getPosition().y + barrier.getSize().y / 2)
				{
					// Move them back
					playerCharacter.move(playerSpeed * deltaTime, 0.f);
				}
			}
			// Take right key press
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
				(playerCharacter.getPosition().y + characterSize.y / 2 < gameHeight - 5.f))
			{
				playerCharacter.move(playerSpeed * deltaTime, 0.f);

				// Check if in barrier
				if (playerCharacter.getPosition().x + characterSize.x / 2 > barrier.getPosition().x - barrier.getSize().x / 2 &&
					playerCharacter.getPosition().x - characterSize.x / 2 < barrier.getPosition().x + barrier.getSize().x / 2 &&
					playerCharacter.getPosition().y + characterSize.y / 2 > barrier.getPosition().y - barrier.getSize().y / 2 &&
					playerCharacter.getPosition().y - characterSize.y / 2 < barrier.getPosition().y + barrier.getSize().y / 2)
				{
					// Move them back	
					playerCharacter.move(-playerSpeed * deltaTime, 0.f);
				}
			}

			// Move opponant player
			// Set up the start and end positions for the lerp
			sf::Vector2f first_pos = opponentCharacter.getPosition();
			sf::Vector2f second_pos = predicted_position;

			// Find vector between points
			sf::Vector2f direction = second_pos - first_pos;

			// Lerp between positions
			sf::Vector2f set_pos = (first_pos + (direction * lerp_timer.getElapsedTime().asSeconds()) * 5.0f);
			// Place opponant at lerped position
			opponentCharacter.setPosition(set_pos);

		}

		// Clear the window
		window.clear(sf::Color(50, 200, 50));

		// If the game is running
		if (current_state == PLAYING)
		{
			// Draw the players
			window.draw(playerCharacter);
			window.draw(opponentCharacter);
			
			// Draw barrier and ball
			window.draw(barrier);
			window.draw(ball);

			// UI
			window.draw(player1UI);
			window.draw(player2UI);
		}
		// If the game has not started
		else if (current_state == INACTIVE)
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}
		// If the player is wating to start
		else if (current_state == LOBBY)
		{
			// Draw the pause message
			window.draw(lobbyMessage);
		}

		// Display things on screen			   
		window.display();					   
	}										   
	
	// Return
	return EXIT_SUCCESS;					   
}											   
				
sf::Vector2f predict_position()						   
{				
	// The position the opponant is predicted to go to 
	sf::Vector2f predicted_position;

	// If there are enough positions and they both have different time stamps
	if (position_queue.size() >= 2 && (position_queue.at(1).time != position_queue.at(0).time))
	{					
		// Linear model
		float velocityX = (position_queue.at(1).x - position_queue.at(0).x) / (position_queue.at(1).time - position_queue.at(0).time);
		float velocityY = (position_queue.at(1).y - position_queue.at(0).y) / (position_queue.at(1).time - position_queue.at(0).time);

		// Find predicted position
		predicted_position.x = (velocityX * (game_timer.getElapsedTime().asSeconds() - position_queue.at(1).time)) + position_queue.at(1).x;
		predicted_position.y = (velocityY * (game_timer.getElapsedTime().asSeconds() - position_queue.at(1).time)) + position_queue.at(1).y;

		// Return position
		return predicted_position;
	}
	// If there have not yet been enough times or they have the same time stamps
	else
	{
		// Set prediction to last known position
		predicted_position.x = position_queue.back().x;
		predicted_position.y = position_queue.back().y;

		// Return position
		return predicted_position;
	}
}