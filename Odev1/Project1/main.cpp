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

using namespace sf;
using namespace std;

const float pi = 3.14159f;

RenderWindow window;

Font font;
Text gameoverText;
Text lifeText;
Text scoreText;

Clock gameClock;
float deltaTime;

float frameWidth = 380;
float frameHeight = 620;

bool isPlaying = false;
bool gameover = false;
bool win = false;

int life = 3;
int level = 0;
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

SoundBuffer hitPaddleBuf;
SoundBuffer destroyBrickBuf;
SoundBuffer damageBrickBuf;
SoundBuffer bounceWallBuf;
SoundBuffer dieBuf;
SoundBuffer winBuf;
SoundBuffer loseBuf;
SoundBuffer BGMbuf;
Sound hitPaddleSound;
Sound destroyBrickSound;
Sound damageBrickSound;
Sound bounceWallSound;
Sound dieSound;
Sound winSound;
Sound loseSound;
Sound BGMSound;

vector<Brick*> bricks;

void Initiate();
void Reset();
void Update();
void Render();
void HandleInput();
void loadLevel(int level);

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
	loadLevel(0);
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
	int rBall, rBats, rBricks;

	/* initialize random seed: */
	srand(time(NULL));

	/* generate secret number between 1 and 10: */
	rBall = rand() % 6 + 1;
	rBats = rand() % 5 + 1;

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
	switch (rBricks) {
	case 1:
		textureBrick.loadFromFile("brick_red.png");
		break;
	case 2:
		textureBrick.loadFromFile("brick_blue.png");
		break;
	case 3:
		textureBrick.loadFromFile("brick_pink.png");
		break;
	case 4:
		textureBrick.loadFromFile("brick.png");
		break;
	}



	font.loadFromFile("consola.ttf");

	textureBack.loadFromFile("back.png");

	BGMbuf.loadFromFile("BGM.flac");
	BGMSound.setBuffer(BGMbuf);
	BGMSound.setLoop(true);
	BGMSound.play();

	hitPaddleBuf.loadFromFile("hitPaddle.wav");
	destroyBrickBuf.loadFromFile("destroyBrick.wav");
	damageBrickBuf.loadFromFile("damageBrick.wav");
	bounceWallBuf.loadFromFile("bounceWall.wav");
	dieBuf.loadFromFile("die.wav");
	winBuf.loadFromFile("win.wav");
	loseBuf.loadFromFile("lose.wav");

	hitPaddleSound.setBuffer(hitPaddleBuf);
	destroyBrickSound.setBuffer(destroyBrickBuf);
	damageBrickSound.setBuffer(damageBrickBuf);
	bounceWallSound.setBuffer(bounceWallBuf);
	dieSound.setBuffer(dieBuf);
	winSound.setBuffer(winBuf);
	loseSound.setBuffer(loseBuf);


	background.setSize(Vector2f(frameWidth, frameHeight));
	background.setPosition(0, 0);
	background.setTexture(&textureBack);


	lifeText.setFont(font);
	lifeText.setCharacterSize(20);
	lifeText.setPosition(120, frameHeight - 30);
	lifeText.setString("life:" + std::to_string(life));

	gameoverText.setFont(font);
	gameoverText.setCharacterSize(35);
	gameoverText.setPosition(100, 400);
	gameoverText.setString("");

	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setPosition(80, frameHeight - 30);
	scoreText.setString("score:" + std::to_string(score));

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
		hitPaddleSound.play();
	}
	//bricks
	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (bricks[i]->speed != 0.f)
			{
				if (bricks[i]->picture.getPosition().x - bricks[i]->picture.getSize().x / 2 < 50.f)
					bricks[i]->moveLeft = false;
				else if (bricks[i]->picture.getPosition().x + bricks[i]->picture.getSize().x / 2 > frameWidth - 50.f)
					bricks[i]->moveLeft = true;

				if (bricks[i]->moveLeft)
					bricks[i]->picture.move(-bricks[i]->speed * deltaTime, 0.0f);
				else
					bricks[i]->picture.move(bricks[i]->speed * deltaTime, 0.0f);


			}


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
		BGMSound.pause();
		loseSound.play();
		gameoverText.setString("game over, press \"Enter\" restart");
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
		BGMSound.pause();
		winSound.play();
		gameoverText.setString("Win! press \"Enter\" to next level");
	}
	lifeText.setString("life:" + std::to_string(life));
	scoreText.setString("score:" + std::to_string(score));
}

void Render()
{
	window.clear(sf::Color::Black);
	window.draw(background);
	window.draw(paddle.picture);
	window.draw(ball.picture);

	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->enable)
		{
			if (bricks[i]->hp == 1)
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(0, 255, 255, 255));
			}
			else if (bricks[i]->hp == 2)
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(255, 0, 0, 255));
			}
			else
			{
				bricks[i]->picture.setTexture(&textureBrick);
				bricks[i]->picture.setFillColor(Color::Color(255, 255, 255, 255));
			}
			window.draw(bricks[i]->picture);
		}

	}
	window.draw(lifeText);
	window.draw(gameoverText);
	window.draw(scoreText);
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
		else if (event.type == sf::Event::MouseMoved && !gameover && !win)
		{
			if (Mouse::getPosition(window).x < (frameWidth - 100.f) && Mouse::getPosition(window).x  > 100.f)
			{
				paddle.picture.setPosition(Vector2f(event.mouseMove.x, paddle.picture.getPosition().y));
			}
			if (!isPlaying)
			{
				ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
			}
		}
	}



	if (!gameover)
	{
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
			(paddle.picture.getPosition().x - paddle.picture.getSize().x / 2.f > 50.f))
		{
			paddle.picture.move(-paddle.speed * deltaTime, 0.f);
		}
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
			(paddle.picture.getPosition().x + paddle.picture.getSize().x / 2.f < frameWidth - 50.f))
		{
			paddle.picture.move(paddle.speed * deltaTime, 0.f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Left))
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
			loadLevel(level);
			BGMSound.play();
		}
		else if (win)
		{
			win = false;
			level = (level + 1) % 3;
			loadLevel(level);
			BGMSound.play();
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		level = 0;
		loadLevel(level);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		level = 1;
		loadLevel(level);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		level = 2;
		loadLevel(level);
	}

}


void loadLevel(int level)
{
	isPlaying = false;
	gameover = false;


	gameoverText.setString("");

	paddle.initiate();
	paddle.setSize(150, 35);
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

	if (level == 0)
	{


		for (int i = 0; i < 10; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 3 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(30, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + 6 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 10; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 9 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 1;
			bptr->speed = 400;
			bricks.push_back(bptr);

		}

		//�er�eve

		for (int i = 1; i < 12; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 0 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}
		for (int i = 0; i < 12; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i* bptr->picture.getSize().x, startposY + 19 * bptr->picture.getSize().y + bptr->picture.getSize().y /2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}
		for (int i = 0; i < 20; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + 0 * bptr->picture.getSize().x + 0 * bptr->picture.getSize().x, startposY +i  * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}
		for (int i = 0; i < 20; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(30, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + 0 * bptr->picture.getSize().x + 11 * bptr->picture.getSize().x, startposY + i * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}
	}
	else if (level == 1)
	{

		for (int i = 0; i < 5; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 10 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2 - i * bptr->picture.getSize().y);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 5; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x + 5 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 6 * bptr->picture.getSize().y);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x + 4 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x + 4 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 4 * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 2 * bptr->picture.getSize().y);
				bptr->hp = 1;
				bptr->speed = 300;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x + 6 * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}

	}
	else if (level == 2)
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				int temp = rand() % 5;
				if (temp == 0)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 1;
					bricks.push_back(bptr);
				}
				else if (temp == 1)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 2;
					bricks.push_back(bptr);
				}
				else if (temp == 2)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 99999;
					bricks.push_back(bptr);
				}
				else if (temp == 3)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 1;
					bptr->speed = 300;
					bricks.push_back(bptr);
				}

			}
		}

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
