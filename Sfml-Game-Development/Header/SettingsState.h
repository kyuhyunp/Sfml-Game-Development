#ifndef SETTINGS_STATE_H
#define SETTINGS_STATE_H

#include "State.h"
#include "Container.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

#include <SFML/Graphics/Sprite.hpp>


class SettingsState : public State
{
public:
	SettingsState(StateStack& stack, Context context);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

private:
	void updateLabels();
	void addButtonLabel(size_t index, size_t x, size_t y, const std::string& text, Context context);

	sf::Sprite mBackgroundSprite;
	GUI::Container mGUIContainer;
	std::array<GUI::Button::Ptr, 2 * PlayerAction::Count> mBindingButtons;
	std::array<GUI::Label::Ptr, 2 * PlayerAction::Count> mBindingLabels;
};

#endif