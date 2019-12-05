#include "Renderer.h"
#include "PhysicsManager.h"


Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::update(std::vector<gameObject*> gameObjects)
{
	//Does not need to be in event queue, no event for falling, meaning if player is falling but not pressing keys, the falling wouldn't be rendered
	if (gameObjects.size() > 0)
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i]->renderData->position.x = gameObjects[i]->phyData->body->GetPosition().x * 30;
			gameObjects[i]->renderData->position.y = gameObjects[i]->phyData->body->GetPosition().y * 30;

			gameObjects[i]->renderData->shape.setPosition(gameObjects[i]->renderData->position);
		}
	}
}
