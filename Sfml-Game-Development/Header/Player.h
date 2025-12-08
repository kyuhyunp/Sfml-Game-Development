#ifndef PLAYER_H
#define PLAYER_H

#include "Command.h"
#include "KeyBinding.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <map>

// To Delete: Used to include CommandQueue
class CommandQueue;

class Player
{
public:
	typedef PlayerAction::Type Action;

	enum MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};

	Player(sf::TcpSocket* socket, int32_t identifier, const KeyBinding* binding);

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void handleRealtimeNetworkInput(CommandQueue& commands);

	// React to events or realtime state changes received over the network
	void handleNetworkEvent(Action action, CommandQueue& commands);
	void handleNetworkRealtimeChange(Action action, bool actionEnabled);

	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;

	void disableAllRealtimeActions();
	bool isLocal() const;

private:
	void initializeActions();
	void sendRealtimePacket(Player::Action action, bool isPressed);

	const KeyBinding* mKeyBinding;
	std::map<Action, Command> mActionBinding;
	std::map<Action, bool> mActionProxies;
	MissionStatus mCurrentMissionStatus;
	int32_t mIdentifier;
	sf::TcpSocket* mSocket;
};


#endif