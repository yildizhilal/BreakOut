#include "Brick.hpp"


void Brick::initiate()
{
	picture.setFillColor(sf::Color::White);
}
void Brick::setSize(float width, float height)
{
	picture.setSize(sf::Vector2f(width, height));
	picture.setOrigin(picture.getSize() / 2.f);
}
void Brick::setPosition(float x, float y)
{
	picture.setPosition(sf::Vector2f(x, y));
}
bool Brick::hit()
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