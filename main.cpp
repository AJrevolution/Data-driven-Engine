#include <iostream>
#include <Box2D/Box2D.h>
#include "Renderer.h"
#include "PhysicsManager.h"
#include "GameManager.h"
#include "GameEventManager.h"
#include "NetworkManager.h"
#include "UserInterfaceManager.h"
#include "AudioManager.h"
#include <chrono>
#include <ctime>

//prevent unresolved external errors
std::vector<gameObject*> gameObjects;								// Store game objects obtained from lua file
std::vector<gameObject*> events;									// Store events so subsystems can check to see if they need to act 
std::vector<sf::RectangleShape> UserInterfaceManager::menuVec;		// User Interface object image
std::vector<gameObject*> UserInterfaceManager::UIobjects;			// Store UI objects
std::vector<gameObject*> NetworkManager::networkObjects;			// Store game objects and use networking vector instead
//std::vector<gameObject*> GameManager::playerVec;					
//std::vector<gameObject*> player;									
bool GameManager::forLoopCheck = false;								
int GameManager::player2Increment;									 
sf::Sound AudioManager::sound;										



//logic to control game loop 
bool setup = false;
bool networkSetup = false;
bool menu = true;
bool online = false;
bool player2 = false;
bool finSetup = false;
bool keyWasPressed = false;
bool physicsTime = false;
bool renderTime = false;

int playerIncrementer;
int networkIncrementer = 0;




void main()
{
	//INITALISATION------------------------------------------------------------------------------
	//RENDERER - Create window  
	sf::RenderWindow window(sf::VideoMode(800, 600), "Application");

	//event e // UI function UI is handling all movement and creation of objects
	sf::Event e;
	
	//PHYSICS - Create box2d world
	b2Vec2 Gravity(0.0f, 9.8f);
	b2World world(Gravity);

	//SOUND - Load sound
	sf::SoundBuffer buffer;
	AudioManager::start(&buffer);

	//UI - Setup start up menu
	sf::RectangleShape menuImage;
	UserInterfaceManager::setup();
	menuImage.setSize(sf::Vector2f(800, 600));
	menuImage.setPosition(0.0f, 0.0f);
	sf::Texture menuTexture;
	menuTexture.loadFromFile("menu.png");
	menuImage.setTexture(&menuTexture);
	UserInterfaceManager::menuVec.push_back(menuImage);

	
	//GAME LOOP------------------------------------------------------------------------------
	while (window.isOpen())
	{
		window.clear(sf::Color::White);
		//Create assets - Loop over vector of GameManager objects and call CreateBox
		if (setup == true && menu == false)
		{
			//2 checks - online or single player, online will load networking components, single player will not, thus render a different loop
			//Online has separate checks to ensure level will load for both clients
			if (online == true)
			{
				//Incrementer makes sure setup does not load twice, network setup will loop over this several times
				networkIncrementer++;
				if (networkIncrementer == 1)
				{
					NetworkManager::setup();
				}
				networkSetup = true;

				//If network has finished setting up the level for both clients
				if (finSetup == true)
				{
					GameManager::createObject(&world, gameObjects);
					//Work out which player is in the game object vector
					if (player2 == false)
					{
						gameObjects = NetworkManager::networkObjects;
						for (int i = 0; i < gameObjects.size(); i++)
						{
							if (gameObjects[i]->objectName == "player")
							{
								playerIncrementer = i;
							}
						}
					}
					if (player2 == true)
					{
						for (int i = 0; i < gameObjects.size(); i++)
						{
							if (gameObjects[i]->objectName == "player2")
							{
								playerIncrementer = i;
							}

						}
					}

					//These booleans allow for the update loop to include Physics and Renderer 
					setup = false;
					networkSetup = false;
					networkIncrementer = 10;
				}
			}
			//Single player setup
			else
			{
				GameManager::setupObjects(gameObjects, 1);
				GameManager::createObject(&world, gameObjects);

				//Assign player to correct game object
				for (int i = 0; i < gameObjects.size(); i++)
				{
					if (gameObjects[i]->objectName == "player")
					{
						playerIncrementer = i;
					}

				}
				setup = false;
			}
			
		}
		
		

		//poll input 
		if (networkIncrementer < 1 || networkIncrementer == 10)
		{
			window.pollEvent(e);
			if (e.type == sf::Event::KeyPressed)
			{
				if (menu == true)
				{
					GameManager::getState(e, UserInterfaceManager::UIobjects[0], events, &menu);
				}
				if (gameObjects.size() > 0)
				{
					GameManager::getState(e, gameObjects[playerIncrementer], events, &menu);
				}
				keyWasPressed = true;
			}
		}

		if (menu == false )
		{
			//Subsystem updates

			//Single player updates
			if (online == false)
			{
				PhysicsManager::update(gameObjects, &world, &events, &online);

				Renderer::update(gameObjects);

				gameObjects = GameEventManager::update(gameObjects, &world, &playerIncrementer);
			}
			
			//Multiplayer updates - if player is trying to play online
			if (online == true)
			{
				gameObjects = NetworkManager::update(gameObjects, &world, &events, &networkSetup, &player2, &finSetup);
				if (finSetup == true && networkSetup == false)
				{
					PhysicsManager::update(gameObjects, &world, &events, &online); 
					Renderer::update(gameObjects);
				}
			}
			AudioManager::update(gameObjects, &events, &buffer);
			
			//Clear events after all subsystems update
			events.clear();

			//Draw all objects
			if (gameObjects.size() > 0)
			{
				for (int i = 0; i < gameObjects.size(); i++)
				{
					window.draw(gameObjects[i]->renderData->shape);
				}
			}
		}

		//Menu User interface for deciding if network or not
		if (menu == true)
		{
			UserInterfaceManager::update(UserInterfaceManager::UIobjects, &events, &setup, &menu, &online, &player2);
			window.draw(menuImage);
		}

		//update window 
		window.display();
	}
	
}
