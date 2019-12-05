#pragma once

#include "gameObject.h"
#include <vector>
#include "RenderData.h"


class Renderer
{
public:
	
	Renderer();
	~Renderer();
	float halfWidth = 0.5;
	float scalingFactor = 30.0f;
	static void update(std::vector<gameObject*> gameObjects);
	std::vector<gameObject*> *objects;
};


