#include "../Header/Application.h"


const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application(FontHolder& fonts)
	: mWindow(sf::VideoMode({ 640, 480 }), "States", sf::Style::Close)
	, mTextures()
	, mFonts(fonts)
	, mPlayer()
	, mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer))
	, mStatisticsText(fonts.get(Fonts::ID::Sansation))
	, mStatisticsUpdateTime(sf::Time::Zero)
	, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);

	mTextures.load(Textures::ID::TitleScreen, "Media/Textures/TitleScreen.png");

	mStatisticsText.setPosition({ 5.f, 5.f });
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Title);
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
}