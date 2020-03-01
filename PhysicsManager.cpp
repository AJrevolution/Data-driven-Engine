#include "PhysicsManager.h"

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::update(std::vector<gameObject*> gameObjects, b2World *world, std::vector<gameObject*> *events, bool *online)
{
	//Check if anything is in game vector
	if (gameObjects.size() > 0)
	{
		if (events->size() > 0 || *online == true)
		{
			//Check for events
			for (int i = 0; i < events->size(); i++)
			{
				if (events->at(i)->events == LEFT)
				{
					events->at(i)->phyData->body->SetLinearVelocity(b2Vec2(-events->at(i)->speed, movementGravity));
					
					std::cout << "x position: " << events->at(i)->renderData->position.x << std::endl;
				}
				else if (events->at(i)->events == RIGHT)
				{
					events->at(i)->phyData->body->SetLinearVelocity(b2Vec2(events->at(i)->speed, movementGravity));
					std::cout << "x position: " << events->at(i)->renderData->position.x << std::endl;
				}

				else if (events->at(i)->events == UP)
				{
					events->at(i)->phyData->body->SetLinearVelocity(b2Vec2(0.0f, -2.5f));
					std::cout << "y position: " << events->at(i)->renderData->position.y << std::endl;
				}
				else if (events->at(i)->events == NOTHINGONLINE)
				{
					events->at(i)->phyData->body->SetTransform(events->at(i)->phyData->onlinePosition, events->at(i)->phyData->body->GetAngle());
					events->at(i)->phyData->position == events->at(i)->phyData->onlinePosition;
				}
			}
		}
	}
	
	//Variables to simulate world
	int velocityIterations = 8;
	int positionIterations = 3;

	world->Step(1.0f / 480.0f, 8, 3);

	//Update physics Data for game object
	for (int i = 0; i < gameObjects.size(); i++)
	{
		b2Body* temp = gameObjects[i]->phyData->body;

		gameObjects[i]->phyData->position.x = gameObjects[i]->phyData->body->GetPosition().x;
		gameObjects[i]->phyData->position.y = gameObjects[i]->phyData->body->GetPosition().y;
		
	}
}