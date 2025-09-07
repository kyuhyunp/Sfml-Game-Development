#include "../Header/MenuState.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/Button.h"
#include "../Header/MusicPlayer.h"

#include <SFML/Graphics/RenderWindow.hpp>


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mGUIContainer()
{
	auto playButton = std::make_shared<GUI::Button>(context);
	playButton->setPosition({ 100, 250 });
	playButton->setText("Play");
	playButton->setCallback([this] () 
	{
		requestStackPop();
		requestStackPush(States::Game);
	});

	auto settingsButton = std::make_shared<GUI::Button>(context);
	settingsButton->setPosition({ 100, 300 });
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
	{
		requestStackPush(States::Settings);
	});

	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition({ 100, 350 });
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
	{
		requestStackPop();
	});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);

	context.music->play(Music::ID::MenuTheme);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time dt)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}

