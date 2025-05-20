#include "../../Header/Learn/LoadingState.h"
#include "../../Header/ResourceHolder.hpp"
#include "../../Header/Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


LoadingState::LoadingState(StateStack& stack, Context context)
	: State(stack, context)
	, mLoadingText(context.fonts->get(Fonts::ID::Sansation))
	, mProgressBarBackground()
	, mProgressBar()
	, mLoadingTask()
{
	sf::RenderWindow& window = *context.window;
	sf::Vector2f viewSize = window.getView().getSize();

	mLoadingText.setString("Loading Resources");
	centerOrigin(mLoadingText);
	mLoadingText.setPosition({ viewSize.x / 2.f, viewSize.y / 2.f + 50.f });

	mProgressBarBackground.setFillColor(sf::Color::White);
	mProgressBarBackground.setSize({ viewSize.x - 20.f, 10.f });
	mProgressBarBackground.setPosition({ 10.f, mLoadingText.getPosition().y + 40.f });

	mProgressBar.setFillColor(sf::Color(100, 100, 100));
	mProgressBar.setSize({ 200.f, 10.f });
	mProgressBar.setPosition({ 10.f, mLoadingText.getPosition().y + 40.f });

	setCompletion(0.f);

	mLoadingTask.execute();
}

void LoadingState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	window.draw(mLoadingText);
	window.draw(mProgressBarBackground);
	window.draw(mProgressBar);
}

bool LoadingState::update(sf::Time dt)
{
	if (mLoadingTask.isFinished())
	{
		requestStackPop();
		requestStackPush(States::Game);
	}
	else
	{
		setCompletion(mLoadingTask.getCompletion());
	}

	return true;
}

bool LoadingState::handleEvent(const sf::Event& event)
{
	return true;
}

void LoadingState::setCompletion(float percent)
{
	if (percent > 1.f)
	{ // clamp
		percent = 1.f;
	}

	mProgressBar.setSize({ 
		percent * mProgressBarBackground.getSize().x, 
		mProgressBar.getSize().y 
	});
}