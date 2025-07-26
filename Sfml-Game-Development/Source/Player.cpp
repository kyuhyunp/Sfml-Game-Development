#include "../Header/Player.h"
#include "../Header/Aircraft.h"


struct AircraftMover
{
	AircraftMover(float vx, float vy)
		:velocity(vx, vy)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const 
	{
		aircraft.accelerate(velocity);
	}

	sf::Vector2f velocity;
};

Player::Player()
	: mKeyBinding()
	, mActionBinding()
{
	mKeyBinding[sf::Keyboard::Key::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Key::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Key::Up] = MoveUp;
	mKeyBinding[sf::Keyboard::Key::Down] = MoveDown;
	mKeyBinding[sf::Keyboard::Key::Space] = Fire;
	mKeyBinding[sf::Keyboard::Key::M] = LaunchMissile;

	initializeActions();

	for (auto& pair : mActionBinding)
	{
		pair.second.category = Category::PlayerAircraft;
	}
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands) 
{
	if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) 
	{
		auto found = mKeyBinding.find(keyPress->code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBinding[found->second]);
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands) 
{
	for (auto pair : mKeyBinding)
	{
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(mActionBinding[pair.second]);
		}
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}

	return sf::Keyboard::Key::Unknown;
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
	case Fire:
		return true;

	default:
		return false;
	}
}

void Player::initializeActions()
{
	const float playerSpeed = 200.f;

	mActionBinding[MoveLeft].action = derivedAction<Aircraft>(
		AircraftMover(-playerSpeed, 0.f));
	mActionBinding[MoveRight].action = derivedAction<Aircraft>(
		AircraftMover(playerSpeed, 0.f));
	mActionBinding[MoveUp].action = derivedAction<Aircraft>(
		AircraftMover(0.f, -playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Aircraft>(
		AircraftMover(0.f, playerSpeed));
	mActionBinding[Fire].action = derivedAction<Aircraft>(
		[](Aircraft& plane, sf::Time time)
		{
			plane.fire();
		});
	mActionBinding[LaunchMissile].action = derivedAction<Aircraft>(
		[](Aircraft& plane, sf::Time time)
		{
			plane.launchMissile();
		});
}