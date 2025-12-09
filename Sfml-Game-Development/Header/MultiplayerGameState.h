#ifndef MULTIPLAYER_GAMESTATE_H
#define MULTIPLAYER_GAMESTATE_H

#include "State.h"
#include "World.h"
#include "Player.h"
#include "GameServer.h"
#include "NetworkProtocol.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>

// Client
class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool isHost);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	virtual void onActivate();
	void onDestroy();

	void disableAllRealtimeActions();

private:
	void updateBroadcastMessage(sf::Time elapsedTime);
	void handlePacket(int32_t packetType, sf::Packet& packet);

	typedef std::unique_ptr<Player> PlayerPtr;

	World mWorld;
	sf::RenderWindow& mWindow;
	TextureHolder& mTextureHolder;

	std::map<int32_t, PlayerPtr> mPlayers;
	std::vector<int32_t> mLocalPlayerIdentifiers;
	sf::TcpSocket mSocket;
	bool mConnected;
	std::unique_ptr<GameServer> mGameServer;
	sf::Clock mTickClock;

	std::vector<std::string> mBroadcasts;
	sf::Text mBroadcastText;
	sf::Time mBroadcastElapsedTime;

	sf::Text mPlayerInvitationText;
	sf::Time mPlayerInvitationTime;

	sf::Text mFailedConnectionText;
	sf::Clock mFailedConnectionClock;

	bool mActiveState;
	bool mHasFocus;
	bool mHost;
	bool mGameStarted;
	sf::Time mClientTimeout;
	sf::Time mTimeSinceLastPacket;
};

#endif
