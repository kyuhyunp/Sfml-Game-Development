#include "../Header/SettingsState.h"
#include "../Header/Utility.hpp"
#include "../Header/ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, mBackgroundSprite(context.textures->get(Textures::ID::TitleScreen))
	, mGUIContainer()
{
	for (size_t player = 0; player < 2; ++player)
	{
		addButtonLabel(PlayerAction::MoveLeft, player, 0, "Move Left", context);
		addButtonLabel(PlayerAction::MoveRight, player, 1, "Move Right", context);
		addButtonLabel(PlayerAction::MoveUp, player, 2, "Move Up", context);
		addButtonLabel(PlayerAction::MoveDown, player, 3, "Move Down", context);
		addButtonLabel(PlayerAction::Fire, player, 4, "Fire", context);
		addButtonLabel(PlayerAction::LaunchMissile, player, 5, "Missile", context);
	}

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(context);
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
	bool isKeyBinding = false;
	for (size_t button = 0; button < 2 * PlayerAction::Count; ++button)
	{
		if (mBindingButtons[button]->isActive())
		{
			isKeyBinding = true;
			if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
			{
				if (button < PlayerAction::Count)
				{
					getContext().keys1->assignKey(
						static_cast<PlayerAction::Type>(button), keyReleased->code);
				}
				else
				{
					getContext().keys2->assignKey(
						static_cast<PlayerAction::Type>(button - PlayerAction::Count), 
						keyReleased->code);
				}
				
				mBindingButtons[button]->deactivate();
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

	for (size_t button = 0; button < PlayerAction::Count; ++button)
	{
		auto action = static_cast<PlayerAction::Type>(button);
		
		sf::Keyboard::Key key1 = getContext().keys1->getAssignedKey(action);
		sf::Keyboard::Key key2 = getContext().keys2->getAssignedKey(action);

		mBindingLabels[button]->setText(toString(key1));
		mBindingLabels[button + PlayerAction::Count]->setText(toString(key2));
	}
}

void SettingsState::addButtonLabel(size_t index, size_t x, size_t y, const std::string& text, Context context)
{
	index += PlayerAction::Count * x;

	mBindingButtons[index] = std::make_shared<GUI::Button>(context);
	mBindingButtons[index]->setPosition({ 400.f * x + 80.f, 50.f * y + 300.f });
	mBindingButtons[index]->setText(text);
	mBindingButtons[index]->setToggle(true);

	mBindingLabels[index] = std::make_shared<GUI::Label>("",  * context.fonts);
	mBindingLabels[index]->setPosition({ 400.f * x + 300.f, 50.f * y + 315.f });

	mGUIContainer.pack(mBindingButtons[index]);
	mGUIContainer.pack(mBindingLabels[index]);
}
