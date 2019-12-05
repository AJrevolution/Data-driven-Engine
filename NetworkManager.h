#pragma once
#include <vector>
#include "gameObject.h"
#include <enet/enet.h>
#include "GameManager.h"

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
	static std::vector <gameObject*> networkObjects;
	static std::vector<gameObject*> update(std::vector<gameObject*> gameObjects, b2World *world, std::vector<gameObject*> *events, bool* setup, bool* player2, bool* finSetup);
	static void setup();
};

