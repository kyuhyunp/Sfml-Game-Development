#include "../Header/MenuState.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/Button.h"

#include <SFML/Graphics/RenderWindow.hpp>


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mGUIContainer()
{
	const FontHolder& fonts = *context.fonts;
	const TextureHolder& textures = *context.textures;
	sf::RenderWindow& window = *context.window;

	auto playButton = std::make_shared<GUI::Button>(fonts, textures);
	playButton->setPosition({ 100, 250 });
	playButton->setText("Play");
	playButton->setCallback([this] () 
	{
		requestStackPop();
		requestStackPush(States::Game);
	});

	auto settingsButton = std::make_shared<GUI::Button>(fonts, textures);
	settingsButton->setPosition({ 100, 300 });
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
	{
		requestStackPush(States::Settings);
	});

	auto exitButton = std::make_shared<GUI::Button>(fonts, textures);
	exitButton->setPosition({ 100, 350 });
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
	{
		requestStackPop();
	});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);
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

