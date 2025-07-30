#ifndef GAMEOVER_STATE_H
#define GAMEOVER_STATE_H

#include "State.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>


class GameOverState : public State
{
public:
	GameOverState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event&);

private:
	sf::Text mGameOverText;
	sf::Time mElapsedTime;
};

#endif
