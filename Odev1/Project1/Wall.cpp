#include "Wall.hpp"


void Wall::initiate()
{
	picture.setFillColor(sf::Color::White);
}
void Wall::setSize(float width, float height)
{
	picture.setSize(sf::Vector2f(width, height));
	picture.setOrigin(picture.getSize() / 2.f);
}
void Wall::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}
bool Wall::hit()
{
	hp--;
	if (hp == 0)
	{
		enable = false;
		return true;
	}
	else
	{
		return false;
	}

}