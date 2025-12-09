#include "../Header/Application.h"
#include "../Header/Utility.hpp"
#include "../Header/ResourceHolder.hpp"

#include <iostream>


const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application(FontHolder& fonts)
	: mWindow(sf::VideoMode({ 1024, 768 }), "States", sf::Style::Close)
	, mTextures()
	, mFonts(fonts)
	, mSounds()
	, mMusic()
	, mKeyBinding1(1)
	, mKeyBinding2(2)
	, mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic, mSounds, 
		mKeyBinding1, mKeyBinding2))
	, mStatisticsText(fonts.get(Fonts::ID::Sansation))
	, mStatisticsUpdateTime(sf::Time::Zero)
	, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);

	mTextures.load(Textures::ID::TitleScreen, "Media/Textures/TitleScreen.png");
	mTextures.load(Textures::ID::Buttons, "Media/Textures/Buttons.png");

	mStatisticsText.setPosition({ 5.f, 5.f });
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Title);

	mMusic.setVolume(25.f);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;

		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			if (mStateStack.isEmpty())
			{
				mWindow.close();
			}
		}

		updateStatistics(dt);
		render();
	}
}

void Application::processInput()
{
	while (std::optional<sf::Event> event = mWindow.pollEvent())
	{
		if (!event.has_value())
		{
			continue;
		}

		mStateStack.handleEvent(event.value());

		if (event->is<sf::Event::Closed>())
		{
			mWindow.close();
		}
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
}

void Application::render()
{
	mWindow.clear();

	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);

	mWindow.display();
}

void Application::updateStatistics(sf::Time dt)
{
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.f))
	{
		mStatisticsText.setString("FPS: " + toString(mStatisticsNumFrames));
		mStatisticsUpdateTime -= sf::seconds(1.f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<PauseState>(States::NetworkPause, true);
	mStateStack.registerState<SettingsState>(States::Settings);
	mStateStack.registerState<GameOverState>(States::GameOver, "Mission failed!");
	mStateStack.registerState<GameOverState>(States::MissionSuccess, "Mission successful!");
	mStateStack.registerState<MultiplayerGameState>(States::HostGame, true);
	mStateStack.registerState<MultiplayerGameState>(States::JoinGame, false);
}