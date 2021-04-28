#pragma once
#include <SFML/Graphics.hpp>

class Brick
{
public:
	bool enable = true;
	bool moveLeft = true;

	int hp = 1;
	float speed = 0;
	sf::RectangleShape picture;
	void initiate();
	void setSize(float width, float height);
	void setPosition(float x, float y);
	virtual bool hit();
};

