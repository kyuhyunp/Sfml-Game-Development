#include "../Header/SettingsState.h"
#include "../Header/Utility.hpp"
#include "../Header/ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mGUIContainer()
{
	addButtonLabel(Player::MoveLeft, 300.f, "Move Left", context);
	addButtonLabel(Player::MoveRight, 350.f, "Move Right", context);
	addButtonLabel(Player::MoveUp, 400.f, "Move Up", context);
	addButtonLabel(Player::MoveDown, 450.f, "Move Down", context);
	addButtonLabel(Player::Fire, 500.f, "Fire", context);
	addButtonLabel(Player::LaunchMissile, 550.f, "Missile", context);

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	backButton->setPosition({ 80.f, 620.f });
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
	Player& player = *getContext().player;

	bool isKeyBinding = false;
	for (size_t action = 0; action < Player::ActionCount; ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
			{
				getContext().player->assignKey(
					static_cast<Player::Action>(action), keyReleased->code);
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
