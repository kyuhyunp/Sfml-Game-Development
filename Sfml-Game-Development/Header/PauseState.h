#ifndef PAUSE_STATE_H
#define PAUSE_STATE_H

#include "State.h"
#include "Container.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


class PauseState : public State
{
public:
	PauseState(StateStack& stack, Context context, bool letUpdatesThrough = false);
	~PauseState();

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

private:
	sf::Text mPauseText;
	GUI::Container mGUIContainer;
	bool mLetUpdatesThrough;
};

#endif
