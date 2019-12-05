#include "gameObject.h"



gameObject::gameObject(float density, float friction, b2BodyType type, b2Vec2 position, b2Vec2 size, std::string textureName, std::string objectName, int levelData)
{
	phyData = new PhysicsData();
	renderData = new RenderData();

	phyData->density = density;
	phyData->friction = friction;
	phyData->type = type;
	phyData->position = position;
	phyData->size = size;
	this->textureName = textureName;
	this->objectName = objectName;
	this->levelData = levelData;
}


gameObject::~gameObject()
{
}


void gameObject::CreateBox(b2World *World, gameObject *gameObject)
{
	//Box2d
	//Give Box2D body
	b2BodyDef BodyDef; 

	//Assign given coordinates to body
	BodyDef.position = b2Vec2(gameObject->phyData->position.x / scaler, gameObject->phyData->position.y / scaler); 

	//Define type (dynamic, static etc)
	BodyDef.type = gameObject->phyData->type; 

	//Add body to world
	b2Body* Body = World->CreateBody(&BodyDef); 

	//Create shape for Box2D object
	b2PolygonShape Shape; 
	Shape.SetAsBox((gameObject->phyData->size.x / 2) / scaler, (gameObject->phyData->size.y / 2) / scaler);

	//Create fixture and assign given properties
	b2FixtureDef FixtureDef;
	FixtureDef.density = gameObject->phyData->density;
	if (BodyDef.type == b2_dynamicBody)
	{
		FixtureDef.friction = gameObject->phyData->friction;
	}
	
	//Set shape
	FixtureDef.shape = &Shape; 
	gameObject->phyData->shape = Shape;
	
	//Apply the fixture definition
	Body->CreateFixture(&FixtureDef); 
	gameObject->phyData->body = Body;


	//SFML stuff
	//Set SFML shape given properties and match with Box2D coordinates with correct scaling
	gameObject->renderData->shape.setOrigin(gameObject->phyData->size.x / 2, gameObject->phyData->size.y / 2);
	gameObject->renderData->shape.setSize(sf::Vector2f(gameObject->phyData->size.x, gameObject->phyData->size.y));
	gameObject->renderData->shape.setPosition(sf::Vector2f(gameObject->phyData->body->GetPosition().x * scaler
		, gameObject->phyData->body->GetPosition().y * scaler));
	
	//Assign texture
	sf::Texture texture;
	std::string newtex = gameObject->textureName;
	texture.loadFromFile(gameObject->textureName);
	gameObject->renderData->texture = texture;

	////Assign texture
	//sf::Texture boxTexture;
	//boxTexture.loadFromFile("box.png");
	//gameObject->renderData->texture = boxTexture;

	//Apply texture to object
	gameObject->renderData->shape.setTexture(&gameObject->renderData->texture);

}


//sf::Sprite gameObject::drawObject(gameObject gameObject)
//{
//	return gameObject.renderData.assignedSprite;
//}

