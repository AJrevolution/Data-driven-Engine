#pragma once
#include <SFML\Graphics.hpp>
class RenderData
{
public:
	sf::Vector2f position;
	sf::Sprite assignedSprite;
	sf::RectangleShape shape;
	sf::Texture texture;
};
