#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif


#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "Wall.hpp"
using namespace sf;
using namespace std;

const float pi = 3.14159f;

RenderWindow window;



Clock gameClock;
float deltaTime;

float frameWidth = 420;
float frameHeight = 620;

bool isPlaying = false;
bool gameover = false;
bool win = false;

int life = 3;
int score = 0;
int combo = 0;

const float startposX = 0;
const float startposY = 20;

Paddle paddle;
Ball ball;

Texture textureBall;
RectangleShape background;
Texture textureBack;
Texture texturePaddle;
Texture textureBrick;
Texture textureBrickDamage;
Texture textureWall;


SoundBuffer destroyBrickBuf;
SoundBuffer damageBrickBuf;
SoundBuffer bounceWallBuf;
SoundBuffer dieBuf;
SoundBuffer winBuf;
SoundBuffer loseBuf;
SoundBuffer fon;

Sound destroyBrickSound;
Sound damageBrickSound;
Sound bounceWallSound;
Sound dieSound;
Sound winSound;
Sound loseSound;
Sound fonSound;

vector<Brick*> bricks;
vector<Wall*> wall;
void Initiate();
void Reset();
void Update();
void Render();
void HandleInput();
void loadLevel();

bool BallLeft(RectangleShape rect);
bool BallRight(RectangleShape rect);
bool BallUp(RectangleShape rect);
bool BallBottom(RectangleShape rect);

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);


	window.create(VideoMode(frameWidth, frameHeight), "BREAKOUT");
	Initiate();
	loadLevel();

	while (window.isOpen())
	{
		deltaTime = gameClock.restart().asSeconds();
		HandleInput();


		if (isPlaying && !gameover && !win)
		{
			Update();
		}
		Render();
	}
	return EXIT_SUCCESS;
}


void Initiate()
{
	int rBall, rBats, rBricks,rWall;

	/* initialize random seed: */
	srand(time(0));

	/* generate secret number between 1 and 10: */
	rBall = rand() % 6 + 1;
	rBats = rand() % 5 + 1;
	rWall = rand() % 4 + 1;
	rBricks = rand() % 4 + 1;
	
	switch (rBall) {
	case 1:
		textureBall.loadFromFile("ball_silver.png");
		break;
	case 2:
		textureBall.loadFromFile("ball_blue.png");
		break;
	case 3:
		textureBall.loadFromFile("ball_green.png");
		break;
	case 4:
		textureBall.loadFromFile("ball_red.png");
		break;
	case 5:
		textureBall.loadFromFile("ball_yellow.png");
		break;
	case 6:
		textureBall.loadFromFile("ball_orange.png");
		break;
	}
	switch (rBats) {
	case 1:

		texturePaddle.loadFromFile("bat_black.png");
		break;
	case 2:

		texturePaddle.loadFromFile("bat_blue.png");
		break;
	case 3:

		texturePaddle.loadFromFile("bat_yellow.png");
		break;
	case 4:

		texturePaddle.loadFromFile("bat_orange.png");
		break;
	case 5:

		texturePaddle.loadFromFile("bat_yellow.png");
		break;
	}	
	switch (rWall) {
	case 1:
		textureWall.loadFromFile("brick_red.png");
		break;
	case 2:
		textureWall.loadFromFile("brick_blue.png");
		break;
	case 3:
		textureWall.loadFromFile("brick_pink.png");
		break;
	case 4:
		textureWall.loadFromFile("brick.png");
		break;
	}

	switch (rBricks) {
	case 1:
		textureBrick.loadFromFile("brick_yellow_small.png");
		
		break;
	case 2:
		textureBrick.loadFromFile("brick_blue_small.png");
		
		break;
	case 3:
		textureBrick.loadFromFile("brick_green_small.png");
		;
		break;
	case 4:
		textureBrick.loadFromFile("brick_pink_small.png");
		
		break;
	}

	if (rBricks == 1)
	{
		textureBrickDamage.loadFromFile("brick_yellow_small_cracked.png");
	}

	if (rBricks == 2)
	{
		textureBrickDamage.loadFromFile("brick_blue_small_cracked.png");
	}
	if (rBricks == 3)
	{
		textureBrickDamage.loadFromFile("brick_green_small_cracked.png");
	}
	if (rBricks == 4)
	{
		textureBrickDamage.loadFromFile("brick_pink_small_cracked.png");
	}


	fon.loadFromFile("fon.wav");
	fonSound.setBuffer(fon);
	fonSound.setLoop(true);
	fonSound.play();

	
	destroyBrickBuf.loadFromFile("destroyBrick.wav");
	damageBrickBuf.loadFromFile("damageBrick.wav");
	bounceWallBuf.loadFromFile("bounceWall.wav");
	dieBuf.loadFromFile("die.wav");
	winBuf.loadFromFile("win.wav");
	loseBuf.loadFromFile("lose.wav");

	destroyBrickSound.setBuffer(destroyBrickBuf);
	damageBrickSound.setBuffer(damageBrickBuf);
	bounceWallSound.setBuffer(bounceWallBuf);
	dieSound.setBuffer(dieBuf);
	winSound.setBuffer(winBuf);
	loseSound.setBuffer(loseBuf);
}

void Reset()
{
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
}

void Update()
{
	if (ball.angle < 0)
	{
		ball.angle += 2 * pi;
	}
	ball.angle = fmod(ball.angle, 2 * pi);

	float factor = ball.speed * deltaTime;
	ball.picture.move(std::cos(ball.angle) * factor, std::sin(ball.angle) * factor);
	//physics
	//edge
	if (ball.picture.getPosition().y + ball.picture.getRadius() > frameHeight-30)
	{
		isPlaying = false;
		life--;
		dieSound.play();
		Reset();
	}
	else if (ball.picture.getPosition().x - ball.picture.getRadius() < 50.f)
	{
		ball.angle = pi - ball.angle;
		ball.picture.setPosition(ball.picture.getRadius() + 50.1f, ball.picture.getPosition().y);
		bounceWallSound.play();
	}
	else if (ball.picture.getPosition().x + ball.picture.getRadius() > frameWidth - 50)
	{
		ball.angle = pi - ball.angle;
		ball.setPosition(frameWidth - ball.picture.getRadius() - 50.1f, ball.picture.getPosition().y);
		bounceWallSound.play();
	}
	else if (ball.picture.getPosition().y - ball.picture.getRadius() < 50.f)
	{
		ball.angle = -ball.angle;
		ball.setPosition(ball.picture.getPosition().x, ball.picture.getRadius() + 50.1f);
		bounceWallSound.play();
	}

	//paddle
	if (BallBottom(paddle.picture))
	{
		int dis = ball.picture.getPosition().x - paddle.picture.getPosition().x;
		if (dis > 30 && ball.angle > 1.f / 2.f * pi)
		{
			ball.angle = ball.angle - pi;
		}
		else if (dis < -30 && ball.angle < 1.f / 2.f * pi)
		{
			ball.angle = ball.angle - pi;
		}
		else
		{
			ball.angle = -ball.angle;
			if (ball.angle > 1.f / 2.f * pi && ball.angle < 7.f / 8.f * pi)
			{
				ball.angle += (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle < 1.f / 2.f * pi && ball.angle > 1.f / 8.f * pi)
			{
				ball.angle -= (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle <= 1.f / 8.f * pi)
			{
				ball.angle += (std::rand() % 15) * pi / 180;
			}
			else if (ball.angle >= 7.f / 8.f * pi)
			{
				ball.angle -= (std::rand() % 15) * pi / 180;
			}
		}

		combo = 0;
		ball.setPosition(ball.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
		bounceWallSound.play();
	}
	//bricks
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{

			if (BallUp(bricks[i]->picture))
			{
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y + bricks[i]->picture.getSize().y / 2 + ball.picture.getRadius() + 0.1f);
				if (bricks[i]->hit())
				{
					destroyBrickSound.play();
				}
				else
				{
					damageBrickSound.play();
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallBottom(bricks[i]->picture))
			{
				ball.angle = -ball.angle;
				ball.setPosition(ball.picture.getPosition().x, bricks[i]->picture.getPosition().y - bricks[i]->picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
				if (bricks[i]->hit())
				{
					destroyBrickSound.play();
				}
				else
				{
					damageBrickSound.play();
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallLeft(bricks[i]->picture))
			{
				ball.angle = pi - ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x + ball.picture.getRadius() + bricks[i]->picture.getSize().x / 2 + 0.1f, ball.picture.getPosition().y);
				if (bricks[i]->hit())
				{
					destroyBrickSound.play();
				}
				else
				{
					damageBrickSound.play();
				}
				combo++;
				score = score + combo * 10;
			}
			else if (BallRight(bricks[i]->picture))
			{
				ball.angle = pi - ball.angle;
				ball.setPosition(bricks[i]->picture.getPosition().x - ball.picture.getRadius() - bricks[i]->picture.getSize().x / 2 - 0.1f, ball.picture.getPosition().y);
				if (bricks[i]->hit())
				{
					destroyBrickSound.play();
				}
				else
				{
					damageBrickSound.play();
				}
				combo++;
				score = score + combo * 10;
			}
		}
	}
	if (life <= 0)
	{
		gameover = true;
		fonSound.stop();
		loseSound.play();
	}

	int count = 0;
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable && bricks[i]->hp < 3)
			count++;
	}

	if (count <= 0)
	{
		win = true;
		ball.speed += 100.f;
		winSound.play();
	
	}

}

void Render()
{
	window.clear(sf::Color::Black);
	window.draw(background);
	window.draw(paddle.picture);
	window.draw(ball.picture);

	for (int i = 0; i < wall.size(); ++i)
	{
		if (wall[i]->enable)
		{
			if (wall[i]->hp == 1)
			{
				wall[i]->picture.setTexture(&textureWall);
				wall[i]->picture.setFillColor(Color::Color(0, 255, 255, 255));
			}
			else if (wall[i]->hp == 2)
			{
				wall[i]->picture.setTexture(&textureWall);
				wall[i]->picture.setFillColor(Color::Color(255, 0, 0, 255));
			}
			else
			{
				wall[i]->picture.setTexture(&textureWall);
				wall[i]->picture.setFillColor(Color::Color(255, 255, 255, 255));
			}
			window.draw(wall[i]->picture);
		}

	}

	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (bricks[i]->hp == 1)
			{
				bricks[i]->picture.setTexture(&textureBrickDamage);
			}
			else if (bricks[i]->hp == 2)
			{
				bricks[i]->picture.setTexture(&textureBrick);
			}
			else
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(255, 255, 255, 255));
			}
			window.draw(bricks[i]->picture);
		}
	}
	window.display();
}


void HandleInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			for (int i = 0; i < bricks.size(); ++i)
			{
				delete bricks[i];
				bricks[i] = nullptr;
			}
			bricks.clear();
		}
	}

	if (!gameover)
	{
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
			(paddle.picture.getPosition().x - paddle.picture.getSize().x / 2.f > 30.f))
		{
			paddle.picture.move(-paddle.speed * deltaTime, 0.f);
		}
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
			(paddle.picture.getPosition().x + paddle.picture.getSize().x / 2.f < frameWidth - 30.f))
		{
			paddle.picture.move(paddle.speed * deltaTime, 0.f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			isPlaying = true;
		}

		if (!isPlaying)
		{
			ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
	{
		if (gameover)
		{
			life = 3;
			gameover = false;
			score = 0;
			combo = 0;
			loadLevel();
			fonSound.play();
		}
		else if (win)
		{
			win = false;
			//bu lýsýmda leveli atlatýlýp devam edilebilir.
			loadLevel();
			fonSound.play();
		}
	}
}

// win den sonra geçiþ leveli tasarlanabilir.
void loadLevel()
{
	isPlaying = false;
	gameover = false;


	

	paddle.initiate();
	paddle.setSize(140, 35);
	paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);
	paddle.picture.setTexture(&texturePaddle);

	ball.initiate();
	ball.setSize(10);
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + std::rand() % 60 - 30) * 2 * pi / 360;
	ball.picture.setTexture(&textureBall);


	for (int i = 0; i < bricks.size(); ++i)
	{
		delete bricks[i];
		bricks[i] = nullptr;
	}
	bricks.clear();

		//tuðla
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(85, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 1 + j * bptr->picture.getSize().x, startposY + 1 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		

		//Çerçeve

		for (int i = 1; i < 13; i++)
		{
			Wall* bptr = new Wall;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 0 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			wall.push_back(bptr);

		}
		for (int i = 0; i < 13; i++)
		{
			Wall* bptr = new Wall;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i* bptr->picture.getSize().x, startposY + 19 * bptr->picture.getSize().y + bptr->picture.getSize().y /2);
			bptr->hp = 99999;
			wall.push_back(bptr);

		}
		for (int i = 0; i < 20; i++)
		{
			Wall* bptr = new Wall;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + 0 * bptr->picture.getSize().x + 0 * bptr->picture.getSize().x, startposY +i  * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			wall.push_back(bptr);

		}
		for (int i = 0; i < 20; i++)
		{
			Wall* bptr = new Wall;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + 0 * bptr->picture.getSize().x + 13 * bptr->picture.getSize().x, startposY + i * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			wall.push_back(bptr);

		}
	}

bool BallLeft(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x + ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallRight(RectangleShape rect)
{
	if (ball.picture.getPosition().x - ball.picture.getRadius() > rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() < rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() >= rect.getPosition().y - rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() <= rect.getPosition().y + rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallUp(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y - ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
bool BallBottom(RectangleShape rect)
{
	if (ball.picture.getPosition().x + ball.picture.getRadius() >= rect.getPosition().x - rect.getSize().x / 2 &&
		ball.picture.getPosition().x - ball.picture.getRadius() <= rect.getPosition().x + rect.getSize().x / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() < rect.getPosition().y + rect.getSize().y / 2 &&
		ball.picture.getPosition().y + ball.picture.getRadius() > rect.getPosition().y - rect.getSize().y / 2)
		return true;
	else
		return false;
}
