#include "../Header/PauseState.h"

#include "../Header/Utility.hpp"
#include "../Header/ResourceHolder.hpp"

// Possibly use a RectangleShape to block other views
PauseState::PauseState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mPauseText(context.fonts->get(Fonts::ID::Sansation))
	, mInstructionText(context.fonts->get(Fonts::ID::Sansation))
{ 
	sf::Vector2f viewSize = context.window->getView().getSize();

	mPauseText.setString("Game Paused");
	mPauseText.setCharacterSize(70);
	centerOrigin(mPauseText);
	mPauseText.setPosition({ 0.5f * viewSize.x,  0.4f * viewSize.y });

	mInstructionText.setString("(Press Backspace to return to the main menu)");
	centerOrigin(mInstructionText);
	mInstructionText.setPosition({ 0.5f * viewSize.x,  0.6f * viewSize.y });
}

void PauseState::draw()
{
	// set the view to Default View
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape(window.getView().getSize());
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));

	window.draw(mBackgroundSprite);
	window.draw(mPauseText);
	window.draw(mInstructionText);
}

bool PauseState::update(sf::Time dt)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{ // Play
			requestStackPop();
		}
		else if (keyPressed->code == sf::Keyboard::Key::Backspace)
		{ // Menu
			requestStateClear();
			requestStackPush(States::Menu);
		}
	}

	// Don't let players handle GameState event
	return false;
}