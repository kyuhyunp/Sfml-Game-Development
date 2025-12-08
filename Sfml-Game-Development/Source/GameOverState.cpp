#include "../Header/GameOverState.h"
#include "../Header/Utility.hpp"
#include "../Header/Player.h"
#include "../Header/ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


GameOverState::GameOverState(StateStack& stack, Context context, const std::string& text)
	: State(stack, context)
	, mGameOverText(context.fonts->get(Fonts::ID::Sansation), text, 70)
	, mElapsedTime(sf::Time::Zero)
{
	centerOrigin(mGameOverText);

	sf::Vector2u windowSize = context.window->getSize();
	mGameOverText.setPosition({ 0.5f * windowSize.x, 0.4f * windowSize.y });
}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape(window.getView().getSize());
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));

	window.draw(backgroundShape);
	window.draw(mGameOverText);
}

bool GameOverState::update(sf::Time dt)
{
	mElapsedTime += dt;
	if (mElapsedTime > sf::seconds(3))
	{
		requestStateClear();
		requestStackPush(States::Menu);
	}

	return false;
}

bool GameOverState::handleEvent(const sf::Event&)
{
	return false;
}