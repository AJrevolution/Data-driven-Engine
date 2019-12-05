#pragma once
#include "Box2D/Box2D/Box2D.h"

class PhysicsData
{
public:
	PhysicsData();
	~PhysicsData() {};
	inline void setPosition(b2Vec2 newPos) { position = newPos; };
	b2Vec2 getPosition() { return position; };
	inline void setVelocity(b2Vec2 newVel) { velocity = newVel; };
	b2Vec2 getVelocity() { return velocity; };

	b2Vec2 velocity, position, size, onlinePosition;
	float mass, density, friction;
	static const int realToPixelScaler = 30;
	b2BodyType type;
	b2Body* body;
	b2PolygonShape shape;
};
