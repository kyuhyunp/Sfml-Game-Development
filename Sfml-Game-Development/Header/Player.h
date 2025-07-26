#ifndef PLAYER_H
#define PLAYER_H

#include "CommandQueue.h"
#include <SFML/Window/Event.hpp>

#include <map>


class Player
{
public:

	enum Action 
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,
		ActionCount
	};

	Player();

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

private:
	static bool isRealtimeAction(Action action);

	void initializeActions();

	std::map<sf::Keyboard::Key, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;

};


#endif