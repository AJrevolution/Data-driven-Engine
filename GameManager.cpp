#include "GameManager.h"
#include <iostream>

std::vector<std::string> elementList;
std::vector<std::string> entityList;
std::vector<int> posData;
std::vector<std::string> getElements(const std::string& table, lua_State* L);
//TODO ENUMERATE TEXTURE NAMES

GameManager::GameManager()
{
}


GameManager::~GameManager()
{
}

void GameManager::createObject(b2World *World, std::vector<gameObject*> gameObjects)
{
	for (int ol = 0; ol < gameObjects.size(); ol++)
	{
		//create object
		gameObjects[ol]->CreateBox(World, gameObjects[ol]);	
	}
}

void GameManager::getState(sf::Event e, gameObject* object, std::vector<gameObject*> &events, bool *menu)
{
	//Push onto event queue
	if (*menu == false)
	{
		//Game commands
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			object->events = LEFT;
			events.push_back(object);
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			object->events = RIGHT;
			events.push_back(object);
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			object->events = UP;
			events.push_back(object);
		}

		
	}

	//UI commands
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		object->events = PLAY;
		events.push_back(object);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		object->events = RECEIVE;
		events.push_back(object);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		object->events = SEND;
		events.push_back(object);
	}
}

void GameManager::setupObjects(std::vector<gameObject*> &objectVec, int level)
{
	//Lua preamble
	lua_State* F = luaL_newstate();

	F = luaL_newstate();
	luaL_dofile(F, "elements.lua");
	luaL_openlibs(F);
	lua_pcall(F, 0, 0, 0);

	//element list house data on lua script for comparison
	elementList = getElements("elementList", F);

	luabridge::LuaRef elementsRef = luabridge::getGlobal(F, "elementList");
	int checker;
	currLevel = level;

	//traverse list and store data - script will only read the level that is stated, meaning it will have to reload entire script to progress a level
	for (int i = 0; i < elementList.size(); i++)
	{
		luabridge::LuaRef entityCheck = elementsRef[elementList.at(i)];
		checker = entityCheck["e_type"].cast<int>();
		int leveldetected = entityCheck["level"].cast<int>();
		//Ensure the correct objects for each level load
		if (level == entityCheck["level"].cast<int>())
		{
			if (checker == 1) //dynamic body
			{
				float lposX = entityCheck["posX"].cast<float>();
				float lposY = entityCheck["posY"].cast<float>();

				float lsizeX = entityCheck["sizeX"].cast<float>();
				float lsizeY = entityCheck["sizeY"].cast<float>();
				std::string newString = entityCheck["textureName"].cast<std::string>();
				std::string nameObject = entityCheck["objectName"].cast<std::string>();
				
				objectVec.push_back(new gameObject(entityCheck["density"].cast<float>(),
					entityCheck["friction"].cast<float>(),
					b2_dynamicBody,
					b2Vec2(lposX, lposY),
					b2Vec2(lsizeX, lsizeY),
					newString,
					entityCheck["objectName"].cast<std::string>(),
					entityCheck["level"].cast<int>()));
			}

			if (checker == 2) //static body
			{
				float lposX = entityCheck["posX"].cast<float>();
				float lposY = entityCheck["posY"].cast<float>();

				float lsizeX = entityCheck["sizeX"].cast<float>();
				float lsizeY = entityCheck["sizeY"].cast<float>();

				objectVec.push_back(new gameObject(entityCheck["density"].cast<float>(),
					entityCheck["friction"].cast<float>(),
					b2_staticBody,
					b2Vec2(lposX, lposY),
					b2Vec2(lsizeX, lsizeY),
					entityCheck["textureName"].cast<std::string>(),
					entityCheck["objectName"].cast<std::string>(),
					entityCheck["level"].cast<int>()));
			}
		}

	}

	for (int i = 0; i < entityList.size(); i++)
	{
		std::cout << (std::string)entityList.at(i) << "\n";
	}

	//debug variables for development
	//////objectVec.push_back(new gameObject(1, 0.7, b2_dynamicBody, b2Vec2(500, 0), b2Vec2(16, 16), "box.png", "player"));
	//objectVec.push_back(new gameObject(1, 0.7, b2_staticBody, b2Vec2(500, 200), b2Vec2(500, 16), "box.png", "ground"));
	//objectVec.push_back(new gameObject(1, 0.7, b2_staticBody, b2Vec2(380, 600), b2Vec2(16, 16), "box.png", "test"));
}

std::vector<gameObject*> GameManager::makeGameObject(std::vector<gameObject*> gameObjects ,std::vector<gameObject*> vector, float density, float friction, int type, b2Vec2 position, b2Vec2 size, std::string objectName, std::string textureName, int level)
{
	//For network purposes, 1 represents dynamic body, 2 for static
	if (type == 1)
	{
		vector.push_back(new gameObject(density, friction, b2_dynamicBody, position, size, textureName, objectName, level));
		gameObjects.push_back(new gameObject(density, friction, b2_dynamicBody, position, size, textureName, objectName, level));
	}
	
	else if (type == 2)
	{
		vector.push_back(new gameObject(density, friction, b2_staticBody, position, size, textureName, objectName, level));
		gameObjects.push_back(new gameObject(density, friction, b2_staticBody, position, size, textureName, objectName, level));
	}

	return gameObjects;
	
}

std::vector<gameObject*> GameManager::getDirection(std::vector<gameObject*> gameObjects, std::vector<gameObject*> &events, int direction, bool* player2)
{
	//Determine what key opponent pressed, values received from packet
	if (forLoopCheck == false)
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			//Reverse logic is applied, as the player 1 will be the first person to connect to the server, therefore by the lua script will be marked as player, thus player2 for that person
			//is player 2, whereas for the 2nd player that connects to the server, they will be player 2 (but player 1 in their game), 
			//thus the object marked as player will be player 2 in his game.
			if (*player2 == true)
			{
				if (gameObjects[i]->objectName == "player")
				{
					player2Increment = i;
					forLoopCheck = true;
				}
			}
			else
			{
				if (gameObjects[i]->objectName == "player2")
				{
					player2Increment = i;
					forLoopCheck = true;
				}
			}
		}
	}

	if (direction == 1)
	{
		std::cout << "Up was pressed\n";
		gameObjects[player2Increment]->events = UP;
	}

	else if (direction == 2)
	{
		std::cout << "Left was pressed\n";
		gameObjects[player2Increment]->events = LEFT;
	}

	else if (direction == 3)
	{
		std::cout << "Right was pressed\n";
		gameObjects[player2Increment]->events = RIGHT;
	}
	
	events.push_back(gameObjects[player2Increment]);
	return gameObjects;
}

std::vector<gameObject*> GameManager::forcePosition(std::vector<gameObject*> gameObjects, std::vector<gameObject*> &events, b2Vec2 coords, bool* player2)
{
	//Function will force an object into a specific position
	if (forLoopCheck == false)
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			//Reverse logic is applied, as the player 1 will be the first person to connect to the server, therefore by the lua script will be marked as player, thus player2 for that person
			//is player 2, whereas for the 2nd player that connects to the server, they will be player 2 (but player 1 in their game), 
			//thus the object marked as player will be player 2 in his game.
			if (*player2 == true)
			{
				if (gameObjects[i]->objectName == "player")
				{
					player2Increment = i;
					forLoopCheck = true;
				}
			}
			else
			{
				if (gameObjects[i]->objectName == "player2")
				{
					player2Increment = i;
					forLoopCheck = true;
				}
			}
		}
	}

	gameObjects[player2Increment]->phyData->onlinePosition = coords;
	gameObjects[player2Increment]->events = NOTHINGONLINE;
	events.push_back(gameObjects[player2Increment]);

	return gameObjects;
}

void GameManager::start()
{
	
}

std::vector<gameObject*> GameManager::networkSetup(std::vector<gameObject*> nlevelData, bool *player2)
{
	//Function is alsmot identical to previous setup, however was required to return a vector to make networking subsystem work
	
	lua_State* F = luaL_newstate();

	F = luaL_newstate();
	luaL_dofile(F, "networklevel.lua");
	luaL_openlibs(F);
	lua_pcall(F, 0, 0, 0);

	elementList = getElements("elementList", F);

	luabridge::LuaRef elementsRef = luabridge::getGlobal(F, "elementList");
	int checker;
	int level = 0;
	
	for (int i = 0; i < elementList.size(); i++)
	{
		luabridge::LuaRef entityCheck = elementsRef[elementList.at(i)];
		checker = entityCheck["e_type"].cast<int>();
		int leveldetected = entityCheck["level"].cast<int>();
		//Ensure the correct objects for each level load
		if (level == entityCheck["level"].cast<int>())
		{
			if (checker == 1) //dynamic body
			{
				float lposX = entityCheck["posX"].cast<float>();
				float lposY = entityCheck["posY"].cast<float>();

				float lsizeX = entityCheck["sizeX"].cast<float>();
				float lsizeY = entityCheck["sizeY"].cast<float>();
				std::string newString = entityCheck["textureName"].cast<std::string>();
				std::string nameObject = entityCheck["objectName"].cast<std::string>();

				nlevelData.push_back(new gameObject(entityCheck["density"].cast<float>(),
					entityCheck["friction"].cast<float>(),
					b2_dynamicBody,
					b2Vec2(lposX, lposY),
					b2Vec2(lsizeX, lsizeY),
					newString,
					entityCheck["objectName"].cast<std::string>(),
					entityCheck["level"].cast<int>()));
			}

			if (checker == 2) //static body
			{
				float lposX = entityCheck["posX"].cast<float>();
				float lposY = entityCheck["posY"].cast<float>();

				float lsizeX = entityCheck["sizeX"].cast<float>();
				float lsizeY = entityCheck["sizeY"].cast<float>();

				nlevelData.push_back(new gameObject(entityCheck["density"].cast<float>(),
					entityCheck["friction"].cast<float>(),
					b2_staticBody,
					b2Vec2(lposX, lposY),
					b2Vec2(lsizeX, lsizeY),
					entityCheck["textureName"].cast<std::string>(),
					entityCheck["objectName"].cast<std::string>(),
					entityCheck["level"].cast<int>()));
			}
		}
	}
	return nlevelData;
}

std::vector<gameObject*> GameManager::deleteAllObjects(std::vector<gameObject*> gameObjects, b2World *World)
{
	//Delete bodies from physics world
	for (b2Body* BodyIterator = World->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
	{

		BodyIterator->SetActive(false);
		//BodyIterator->DestroyFixture(BodyIterator->GetFixtureList());
	}

	//Delete rest of objects
	for (int i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	gameObjects.clear();
	currLevel++;

	return gameObjects;
}


std::vector<std::string> getElements(const std::string& tab, lua_State* L)
{
	std::string source =
		"function getElements(tab) "
		"s = \"\""
		"for k, v in pairs(_G[tab]) do "
		"    s = s..k..\"|\" "
		"    end "
		"return s "
		"end";

	luaL_loadstring(L, source.c_str());
	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "getElements");
	lua_pushstring(L, tab.c_str());
	lua_pcall(L, 1, 1, 0);

	std::string ans = lua_tostring(L, -1);
	std::vector<std::string> elements;
	std::string temp = "";
	for (unsigned int i = 0; i < ans.size(); i++)
	{
		if (ans.at(i) != '|')
		{
			temp += ans.at(i);
		}
		else
		{
			elements.push_back(temp);
			temp = "";
		}
	}

	int n = lua_gettop(L);
	lua_pop(L, 1);

	return elements;
}