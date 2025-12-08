#include "../Header/Player.h"
#include "../Header/Aircraft.h"
#include "../Header/NetworkProtocol.h"
#include "../Header/CommandQueue.h"

#include <SFML/Network/Packet.hpp>
#include <iostream>


struct AircraftMover
{
	AircraftMover(float vx, float vy, int32_t identifier)
		: velocity(vx, vy)
		, aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const 
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.accelerate(velocity * aircraft.getMaxSpeed());
		}
	}

	sf::Vector2f velocity;
	int32_t aircraftID;
};

struct AircraftFireTrigger
{
	AircraftFireTrigger(int32_t identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.fire();
		}
	}

	int32_t aircraftID;
};

struct AircraftMissileTrigger
{
	AircraftMissileTrigger(int32_t identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.launchMissile();
		}
	}

	int32_t aircraftID;
};

Player::Player(sf::TcpSocket* socket, int32_t identifier, const KeyBinding* binding)
	: mKeyBinding(binding)
	, mActionBinding()
	, mActionProxies()
	, mCurrentMissionStatus(MissionRunning)
	, mIdentifier(identifier)
	, mSocket(socket)
{
	initializeActions();

	for (auto& pair : mActionBinding)
	{
		pair.second.category = Category::PlayerAircraft;
	}
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands) 
{ 
	if (!isLocal())
	{ // Remote players' events are not handled
		return;
	}

	// Networked realtime changes rely on event for reliability
	// Local realtime changes should not use this method (assert)
	Action action;
	const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
	if (keyPressed && mKeyBinding->checkAction(keyPressed->code, action))
	{
		if (isRealtimeAction(action))
		{ 
			if (mSocket)
			{ // Case 3: Network realtime change (pressed)
				sendRealtimePacket(action, true);
			}
		}

		// Event
		else if (mSocket)
		{ // Case 1: Network connected and event occurred
		// Send a packet to the server
			sf::Packet packet;
			packet << static_cast<int32_t> (Client::PlayerEvent);
			packet << mIdentifier;
			packet << static_cast<int32_t> (action);
			mSocket->send(packet);
		}
		else
		{ // Case 2: local event
			commands.push(mActionBinding[action]);
		}
	}

	const auto* keyReleased = event.getIf<sf::Event::KeyReleased>();
	if (keyReleased 
		&& mKeyBinding->checkAction(keyReleased->code, action)
		&& isRealtimeAction(action)
		&& mSocket)
	{ // Case 3: Network realtime change (released)
		sendRealtimePacket(action, false);
	}
}

void Player::sendRealtimePacket(Player::Action action, bool isPressed)
{
	sf::Packet packet;
	packet << static_cast<int32_t> (Client::PlayerRealtimeChange);
	packet << mIdentifier;
	packet << static_cast<int32_t> (action);
	packet << isPressed;
	mSocket->send(packet);
}

bool Player::isLocal() const
{ 
	return mKeyBinding != nullptr;
}

void Player::disableAllRealtimeActions()
{ 
	std::cout << "Realtime action disabled for " << mIdentifier << "\n";
	for (auto pair : mActionProxies)
	{
		sf::Packet packet;
		packet << static_cast<int32_t> (Client::PlayerRealtimeChange)
			<< mIdentifier << static_cast<int32_t> (pair.first) << false;
		mSocket->send(packet);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands) 
{ 
	if (isLocal())
	{
		std::vector<Action> activeActions = mKeyBinding->getRealtimeActions();
		for (Action action : activeActions)
		{
			commands.push(mActionBinding[action]);
		}
	}
}

void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{ 
	if (!isLocal())
	{
		for (auto pair : mActionProxies)
		{ // The client shouldn't check server's command
			if (pair.second)
			{
				commands.push(mActionBinding[pair.first]);
			}
		}
	}
}

void Player::handleNetworkEvent(Action action, CommandQueue& commands)
{
	commands.push(mActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(Action action, bool actionEnabled)
{ 
	mActionProxies[action] = actionEnabled;
}

void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	mActionBinding[PlayerAction::MoveLeft].action = derivedAction<Aircraft>(
		AircraftMover(-1.f, 0.f, mIdentifier));
	mActionBinding[PlayerAction::MoveRight].action = derivedAction<Aircraft>(
		AircraftMover(1.f, 0.f, mIdentifier));
	mActionBinding[PlayerAction::MoveUp].action = derivedAction<Aircraft>(
		AircraftMover(0.f, -1.f, mIdentifier));
	mActionBinding[PlayerAction::MoveDown].action = derivedAction<Aircraft>(
		AircraftMover(0.f, 1.f, mIdentifier));
	mActionBinding[PlayerAction::Fire].action = derivedAction<Aircraft>(
		AircraftFireTrigger(mIdentifier));
	mActionBinding[PlayerAction::LaunchMissile].action = derivedAction<Aircraft>(
		AircraftMissileTrigger(mIdentifier));
}