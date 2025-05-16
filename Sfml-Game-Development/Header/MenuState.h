#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "State.h"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

	void updateOptionText();
	
private:
	enum OptionNames
	{
		Play,
		Exit,
	};

	sf::Sprite mBackgroundSprite;
	std::vector<sf::Text> mOptions; 
	size_t mOptionIndex;
};

#endif
