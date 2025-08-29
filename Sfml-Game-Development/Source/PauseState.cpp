#include "../Header/PauseState.h"

#include "../Header/Utility.hpp"
#include "../Header/ResourceHolder.hpp"
#include "../Header/Button.h"

#include <memory>


PauseState::PauseState(StateStack& stack, Context context)
	: State(stack, context)
	, mPauseText(context.fonts->get(Fonts::ID::Sansation))
	, mGUIContainer()
{ 
	sf::Vector2f viewSize = context.window->getView().getSize();

	mPauseText.setString("Game Paused");
	mPauseText.setCharacterSize(70);
	centerOrigin(mPauseText);

	sf::Vector2f pausePosition = { 0.5f * viewSize.x,  0.4f * viewSize.y };
	mPauseText.setPosition(pausePosition);

	auto mReturnButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	mReturnButton->setPosition({ pausePosition.x - 100, pausePosition.y + 75.f });
	mReturnButton->setText("Return");
	mReturnButton->setCallback([this]() {
		requestStackPop();
	});

	auto mMenuButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	mMenuButton->setPosition({ pausePosition.x - 100, pausePosition.y + 125.f });
	mMenuButton->setText("Back to menu");
	mMenuButton->setCallback([this]() {
		requestStateClear();
		requestStackPush(States::Menu);
	});

	mGUIContainer.pack(mReturnButton);
	mGUIContainer.pack(mMenuButton);
	/*mInstructionText.setString("(Press Backspace to return to the main menu)");
	centerOrigin(mInstructionText);
	mInstructionText.setPosition({ 0.5f * viewSize.x,  0.6f * viewSize.y });*/
}

void PauseState::draw()
{
	// set the view to Default View
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape(window.getView().getSize());
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));

	window.draw(backgroundShape);
	window.draw(mPauseText);
	window.draw(mGUIContainer);
}

bool PauseState::update(sf::Time dt)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);

	return false;
}