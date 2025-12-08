#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <thread>
#include <map>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>


class GameServer
{
public:
	explicit GameServer(sf::Vector2f battlefieldSize);
	~GameServer();

private:
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket socket;
		sf::Time lastPacketTime;
		std::vector<int32_t> aircraftIdentifiers;
		bool ready;
		bool timedOut;
	};

	struct AircraftInfo
	{
		sf::Vector2f position;
		int32_t hitpoints;
		int32_t missileAmmo;
		std::map<int32_t, bool> realtimeActions;
	};

	typedef std::unique_ptr<RemotePeer> PeerPtr;

	void notifyPlayerSpawn(int32_t aircraftIdentifier);
	void notifyPlayerRealtimeChange(int32_t aircraftIdentifier,
		int32_t action, bool actionEnabled, const RemotePeer& changer);
	void nofifyPlayerEvent(int32_t aircraftIdentifier, int32_t action);

	void setListening(bool enable);
	void executionThread();
	void tick();

	sf::Time now() const;
	void handleIncomingPackets();
	void handleIncomingPacket(sf::Packet& packet,
		RemotePeer& receivingPeer, bool& detectedTimeout);

	void handleIncomingConnections();
	void handleDisconnections();

	void informWorldState(sf::TcpSocket& socket);
	void broadcastMessage(const std::string& message);
	void sendToAll(sf::Packet& packet);
	void updateClientState();

	std::thread mThread;
	sf::Clock mClock;
	sf::TcpListener mListenerSocket;
	bool mListeningState;
	sf::Time mClientTimeoutTime;

	size_t mMaxConnectedPlayers;
	size_t mConnectedPlayers;

	float mWorldHeight;
	sf::FloatRect mBattleFieldRect;
	float mBattleFieldScrollSpeed;

	size_t mAircraftCount;
	std::map<int32_t, AircraftInfo> mAircraftInfo;

	std::vector<PeerPtr> mPeers;
	int32_t mAircraftIdentifierCounter;
	bool mWaitingThreadEnd;

	sf::Time mLastSpawnTime;
	sf::Time mTimeForNextSpawn;

	std::mutex mInitMutex;
	std::condition_variable mInitCondition;
	bool mInitComplete = false;
};

#endif

