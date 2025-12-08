#ifndef KEY_BINDING_H
#define KEY_BINDING_H

#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <map>

namespace PlayerAction
{
	enum Type
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,
		Count
	};
}

class KeyBinding
{
public:
	typedef PlayerAction::Type Action;

	// controlPreconfiguration notes whether the player is 1 or 2
	explicit KeyBinding(int controlPreconfiguration);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

	bool checkAction(sf::Keyboard::Key key, Action& out) const;
	std::vector<Action> getRealtimeActions() const;

private:
	std::map<sf::Keyboard::Key, Action> mKeyMap;
};

bool isRealtimeAction(PlayerAction::Type action);

#endif

