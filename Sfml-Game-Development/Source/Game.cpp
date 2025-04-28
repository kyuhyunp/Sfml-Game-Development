#include "../Header/Game.hpp"
#include "../Header/StringHelpers.hpp"

#include <iostream>
#include <exception>

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game::Game()
	: mWindow(sf::VideoMode({ 640, 480 }), "SFML Application", sf::Style::Close)
, mTexture()
, mPlayer(mTexture)
, mFont()
, mStatisticsText(mFont)
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
, mIsMovingUp(false)
, mIsMovingDown(false)
, mIsMovingRight(false)
, mIsMovingLeft(false)
{
	if (!mTexture.loadFromFile("Media/Textures/Eagle.png"))
	{
		throw std::runtime_error("Error while opening Media/Textures/Eagle.png");
	}

	mPlayer.setTexture(mTexture, true);
	mPlayer.setPosition({ 100.f, 100.f });
	
	if (!mFont.openFromFile("Media/Sansation.ttf"))
	{
		throw std::runtime_error("Error while opening Media/Textures/Eagle.png");
	}
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition({ 5.f, 5.f });
	mStatisticsText.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	while (const std::optional event = mWindow.pollEvent())
	{
		if (event->is< sf::Event::Closed>()){
			mWindow.close();
			return;
		}

		if (const auto * keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			handlePlayerInput(keyPressed->code, true);
		}
		else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			handlePlayerInput(keyReleased->code, false);
		} 
	}
}

void Game::update(sf::Time elapsedTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;
		
	mPlayer.move(movement * elapsedTime.asSeconds());
}

void Game::render()
{
	mWindow.clear();	
	mWindow.draw(mPlayer);
	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");
							 
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{	
	if (key == sf::Keyboard::Key::W)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Key::S)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Key::A)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Key::D)
		mIsMovingRight = isPressed;
}
