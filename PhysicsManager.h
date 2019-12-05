#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include "gameObject.h"
#include "PhysicsData.h"

class PhysicsManager
{
public:
	PhysicsManager() {};
	~PhysicsManager();
	static const int scale = 30;

	//inline PhysicsData* getElement(int i) { return physicsData.at(i); }
	//void addElement(PhysicsData* element);
	static b2World createWorld(b2Vec2 Gravity);
	static void SimulateWorld(b2World *world);
	//void addObjectsToWorld();
	static void update(std::vector<gameObject*> gameObjects, b2World *world, std::vector<gameObject*> *events, bool *online);
	float recentSpeed = 10;
	static const int movementGravity = 9;
	
private:
	//std::vector<PhysicsData*> physicsData;
};
