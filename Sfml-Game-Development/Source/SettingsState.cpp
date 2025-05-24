#include "../Header/SettingsState.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mGUIContainer()
{
	addButtonLabel(Player::MoveLeft, 150.f, "Move Left", context);
	addButtonLabel(Player::MoveRight, 200.f, "Move Right", context);
	addButtonLabel(Player::MoveUp, 250.f, "Move Up", context);
	addButtonLabel(Player::MoveDown, 300.f, "Move Down", context);

	updateLabels();
	/*const FontHolder& fonts = *context.fonts;
	const TextureHolder& textures = *context.textures;*/

	auto backButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	backButton->setPosition({ 80.f, 375.f });
	backButton->setText("Back");
	backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

	mGUIContainer.pack(backButton);
}

void SettingsState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool SettingsState::update(sf::Time dt)
{
	return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
	// Allow the Player to change keys 
	// pressed (toggle) works
	// Now, to set the key, you need to check if any button is toggled
	Player& player = *getContext().player;

	bool isKeyBinding = false;
	for (size_t action = 0; action < Player::ActionCount; ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
			{
				getContext().player->assignKey(static_cast<Player::Action>(action), keyReleased->code);
				mBindingButtons[action]->deactivate();
			}
			break;
		}
	}

	if (isKeyBinding) 
	{
		updateLabels();
	}
	else
	{
		mGUIContainer.handleEvent(event);
	}

	return false;
}

void SettingsState::updateLabels()
{
	Player& player = *getContext().player;

	for (size_t i = 0; i < Player::ActionCount; ++i)
	{
		sf::Keyboard::Key key = player.getAssignedKey(static_cast<Player::Action>(i));
		mBindingLabels[i]->setText(toString(key));
	}
}

void SettingsState::addButtonLabel(Player::Action action, float y, const std::string& text, Context context)
{

	mBindingButtons[action] = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	mBindingButtons[action]->setPosition({ 80.f, y });
	mBindingButtons[action]->setText(text);
	mBindingButtons[action]->setToggle(true);

	mBindingLabels[action] = std::make_shared<GUI::Label>("",  * context.fonts);
	mBindingLabels[action]->setPosition({ 300.f, y + 15.f });

	mGUIContainer.pack(mBindingButtons[action]);
	mGUIContainer.pack(mBindingLabels[action]);
}
