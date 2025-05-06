#include "../Header/Game.hpp"
#include "../Header/StringHelpers.hpp"

#include <iostream>
#include <exception>

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game::Game(const FontHolder& fonts)
	: mWindow(sf::VideoMode({ 640, 480 }), "World", sf::Style::Close)
	, mWorld(mWindow)
	, mFonts(fonts)
	, mStatisticsText(fonts.get(Fonts::ID::Sansation))
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
{
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
		if (event->is< sf::Event::Closed>())
		{
			mWindow.close();
			break;
		}
		else if (const auto * keyPressed = event->getIf<sf::Event::KeyPressed>()) 
		{
			handlePlayerInput(keyPressed->code, true);
			break;
		}
		else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) 
		{
			handlePlayerInput(keyReleased->code, false);
			break;
		} 
	}
}

void Game::update(sf::Time elapsedTime)
{
	mWorld.update(elapsedTime);
}

void Game::render()
{
	mWindow.clear();	
	mWorld.draw();

	mWindow.setView(mWindow.getDefaultView());
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
}
