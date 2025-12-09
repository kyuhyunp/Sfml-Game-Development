#include "../Header/GameServer.h"
#include "../Header/NetworkProtocol.h"
#include "../Header/Pickup.h"
#include "../Header/Aircraft.h" 

#include <SFML/Network/Packet.hpp>
#include <iostream>


GameServer::RemotePeer::RemotePeer()
	: socket()
	, lastPacketTime(sf::Time::Zero)
	, aircraftIdentifiers()
	, ready(false)
	, timedOut(false)
{
	socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefieldSize)
	: mThread(&GameServer::executionThread, this)
	, mClock()
	, mListenerSocket()
	, mListeningState(false)
	, mClientTimeoutTime(sf::seconds(3.f))
	, mMaxConnectedPlayers(10)
	, mConnectedPlayers(0)
	, mWorldHeight(5000.f)
	, mBattleFieldRect({0.f, mWorldHeight - battlefieldSize.y}, battlefieldSize)
	, mBattleFieldScrollSpeed(-50.f)
	, mAircraftCount(0)
	, mAircraftInfo()
	, mPeers(1)
	, mAircraftIdentifierCounter(1)
	, mWaitingThreadEnd(false)
	, mLastSpawnTime(sf::Time::Zero)
	, mTimeForNextSpawn(sf::seconds(5.f))
	, mInitMutex()
	, mInitCondition()
	, mInitComplete(false)
{
	{
		std::unique_lock<std::mutex> lock(mInitMutex);

		mListenerSocket.setBlocking(false);
		mPeers[0].reset(new RemotePeer());

		mInitComplete = true;
	}

	mInitCondition.notify_one();
}

GameServer::~GameServer()
{ 
	mWaitingThreadEnd = true;
	mThread.join();
}

void GameServer::notifyPlayerSpawn(int32_t aircraftIdentifier)
{ 
	sf::Packet packet;
	packet << static_cast<int32_t>(Server::PlayerConnect);
	packet << aircraftIdentifier 
		<< mAircraftInfo[aircraftIdentifier].position.x
		<< mAircraftInfo[aircraftIdentifier].position.y;

	sendToAll(packet);
}

void GameServer::notifyPlayerRealtimeChange(int32_t aircraftIdentifier,
	int32_t action, bool actionEnabled, const RemotePeer& changer)
{ 
	sf::Packet packet;
	packet << static_cast<int32_t> (Server::PlayerRealtimeChange)
		<< aircraftIdentifier << action << actionEnabled;

	for (auto& peer : mPeers)
	{
		if (peer->ready && peer.get() != &changer)
		{
			peer->socket.send(packet);
		}
	}
}

void GameServer::nofifyPlayerEvent(int32_t aircraftIdentifier, int32_t action)
{ 
	sf::Packet packet;
	packet << static_cast<int32_t> (Server::PlayerEvent);
	packet << aircraftIdentifier << action;
	sendToAll(packet);
}

void GameServer::setListening(bool enable)
{ 
	if (enable)
	{
		if (!mListeningState)
		{
			mListeningState = 
				(mListenerSocket.listen(ServerPort) == sf::TcpListener::Status::Done);
		}
	}
	else
	{
		mListenerSocket.close();
		mListeningState = false;
	}
}

void GameServer::executionThread()
{ 
	{
		std::unique_lock<std::mutex> lock(mInitMutex);
		mInitCondition.wait(lock, [this] { return mInitComplete; });
	}

	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;

	sf::Clock clock;
	while (!mWaitingThreadEnd)
	{
		handleIncomingPackets();
		handleIncomingConnections();

		sf::Time dt = clock.getElapsedTime();
		clock.restart();

		stepTime += dt;
		tickTime += dt;

		while (stepTime >= stepInterval)
		{
			mBattleFieldRect.position.y += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		sf::sleep(sf::milliseconds(1));
	}
}

void GameServer::tick()
{ 
	// Server::UpdateClientState
	updateClientState();

	// Server::MissionSuccess
	bool success = true;
	bool allDestroyed = true;
	for (auto pair : mAircraftInfo)
	{
		allDestroyed = false;

		if (pair.second.position.y > 0.f)
		{
			success = false;
		}
	}

	if (!allDestroyed && success)
	{
		sf::Packet successPacket;
		successPacket << static_cast<int32_t> (Server::MissionSuccess);
		sendToAll(successPacket);
	}

	// Remove destroyed aircraft (Position matters more than HP)
	for (auto itr = mAircraftInfo.begin(); itr != mAircraftInfo.end();)
	{
		if (itr->second.hitpoints <= 0)
		{
			itr = mAircraftInfo.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Server::SpawnEnemy
	if (now() >= mLastSpawnTime + mTimeForNextSpawn)
	{
		if (mBattleFieldRect.position.y > 600.f)
		{
			size_t enemyCount = 1u + rand() % 2;
			float spawnCenterX = static_cast<float> (rand() % 500 - 250);

			// Choose enemy type and position and send them
			float planeDistance = 0.f;
			float nextSpawnX = spawnCenterX;

			if (enemyCount == 2)
			{
				planeDistance = static_cast<float> (150 + rand() % 250);
				nextSpawnX = spawnCenterX - planeDistance / 2.f;
			}

			for (size_t i = 0; i < enemyCount; ++i)
			{
				sf::Packet enemyPacket;
				enemyPacket << static_cast<int32_t>(Server::SpawnEnemy)
					<< static_cast<int32_t>(1 + rand() % (Aircraft::TypeCount - 1))
					<< mWorldHeight - mBattleFieldRect.position.y + 500
					<< nextSpawnX;

				nextSpawnX += planeDistance;

				sendToAll(enemyPacket);
			}
			
			mLastSpawnTime = now();
			mTimeForNextSpawn = sf::milliseconds(2000 + rand() % 6000);
		}
	}
}

sf::Time GameServer::now() const
{
	return mClock.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
	bool detectedTimeOut = false;

	for (PeerPtr& peer : mPeers)
	{
		if (peer->ready)
		{
			sf::Packet packet;
			while (peer->socket.receive(packet) == sf::TcpSocket::Status::Done)
			{
				handleIncomingPacket(packet, *peer, detectedTimeOut);

				peer->lastPacketTime = now();
				packet.clear();
			}
			
			if (peer->lastPacketTime + mClientTimeoutTime <= now())
			{
				detectedTimeOut = true;
				peer->timedOut = true;
			}
		}
	}

	if (detectedTimeOut)
	{
		handleDisconnections();
	}
}

void GameServer::handleIncomingPacket(sf::Packet& packet,
	RemotePeer& receivingPeer, bool& detectedTimeout)
{
	int32_t packetType;
	packet >> packetType;

	switch (packetType)
	{
	case Client::PlayerEvent:
	{ 
		int32_t identifier;
		int32_t action;
		packet >> identifier >> action;

		nofifyPlayerEvent(identifier, action);

		std::cout << "Server Received Client::PlayerEvent\n";
	} break;

	case Client::PlayerRealtimeChange:
	{ 
		int32_t identifier;
		int32_t action;
		bool actionEnabled;
		packet >> identifier >> action >> actionEnabled;

		mAircraftInfo[identifier].realtimeActions[action] = actionEnabled;
		notifyPlayerRealtimeChange(identifier, action, actionEnabled, receivingPeer);

		std::cout << "Server Received Client::PlayerRealtimeChange\n";
	} break;

	case Client::RequestCoopPartner:
	{ 
		// Set up data
		mAircraftInfo[mAircraftIdentifierCounter].position = mBattleFieldRect.getCenter();
		mAircraftInfo[mAircraftIdentifierCounter].hitpoints = 100;
		mAircraftInfo[mAircraftIdentifierCounter].missileAmmo = 2;

		receivingPeer.aircraftIdentifiers.push_back(mAircraftIdentifierCounter);

		// Network to the peer that requested 
		sf::Packet acceptPacket;
		acceptPacket << static_cast<int32_t> (Server::AcceptCoopPartner);
		acceptPacket << mAircraftIdentifierCounter
			<< mAircraftInfo[mAircraftIdentifierCounter].position.x
			<< mAircraftInfo[mAircraftIdentifierCounter].position.y;
		receivingPeer.socket.send(acceptPacket);

		// Network to the other peers
		sf::Packet connectPacket;
		connectPacket << static_cast<int32_t> (Server::PlayerConnect);
		connectPacket << mAircraftIdentifierCounter
			<< mAircraftInfo[mAircraftIdentifierCounter].position.x
			<< mAircraftInfo[mAircraftIdentifierCounter].position.y;

		for (PeerPtr& peer : mPeers)
		{
			if (peer->ready && 
				peer.get() != &receivingPeer)
			{
				peer->socket.send(connectPacket);
			}
		}

		// Finalize global state update
		++mAircraftIdentifierCounter;
		++mAircraftCount;

		std::cout << "Server Received Client::RequestCoopPartner\n";
	} break;

	case Client::PositionUpdate:
	{ 
		int32_t numAircrafts;
		packet >> numAircrafts;

		int32_t identifier;
		for (int32_t i = 0; i < numAircrafts; ++i)
		{ // Should validate before storing
			packet >> identifier;
			packet >> mAircraftInfo[identifier].position.x 
				>> mAircraftInfo[identifier].position.y
				>> mAircraftInfo[identifier].hitpoints
				>> mAircraftInfo[identifier].missileAmmo;
		}
	} break;

	case Client::GameEvent:
	{ 
		int32_t type;
		sf::Vector2f position;
		packet >> type >> position.x >> position.y;

		// Avoid sending multiple pickups by checking receivingPeer
		if (type == GameActions::EnemyExplode && rand() % 3 == 0 &&
			&receivingPeer == mPeers[0].get())
		{
			sf::Packet pickupPacket;
			pickupPacket << static_cast<int32_t> (Server::SpawnPickup)
				<< static_cast<int32_t> (rand() % Pickup::TypeCount)
				<< position.x << position.y;

			sendToAll(pickupPacket);
		}

		std::cout << "Server Received Client::GameEvent\n";
	} break;

	case Client::Quit:
	{ 
		receivingPeer.timedOut = true;
		detectedTimeout = true;

		std::cout << "Server Received Client::Quit\n";
	} break;

	default:
	{ 
	} break;
	}
}

void GameServer::handleIncomingConnections()
{
	if (!mListeningState)
	{
		return;
	}

	// mListenerSocket is used for incoming connections from players
	if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket)
		== sf::TcpListener::Status::Done)
	{
		mAircraftInfo[mAircraftIdentifierCounter].position = mBattleFieldRect.getCenter();
		mAircraftInfo[mAircraftIdentifierCounter].hitpoints = 100;
		mAircraftInfo[mAircraftIdentifierCounter].missileAmmo = 2;

		// Prepare SpawnSelf
		sf::Packet packet;
		packet << static_cast<int32_t> (Server::SpawnSelf);
		packet << mAircraftIdentifierCounter;
		packet << mAircraftInfo[mAircraftIdentifierCounter].position.x;
		packet << mAircraftInfo[mAircraftIdentifierCounter].position.y;

		mPeers[mConnectedPlayers]->aircraftIdentifiers.push_back(mAircraftIdentifierCounter);

		// Broadcast message
		broadcastMessage("New Player!");

		// InitialState
		informWorldState(mPeers[mConnectedPlayers]->socket);

		// PlayerConnect
		notifyPlayerSpawn(mAircraftIdentifierCounter++);

		// SpawnSelf
		mPeers[mConnectedPlayers]->socket.send(packet);

		// Update connection info
		mPeers[mConnectedPlayers]->ready = true;
		mPeers[mConnectedPlayers]->lastPacketTime = now();

		mAircraftCount++;
		mConnectedPlayers++;
		
		if (mConnectedPlayers < mMaxConnectedPlayers)
		{
			mPeers.push_back(PeerPtr(new RemotePeer()));
		}
		else
		{
			setListening(false);
		}
	}
}

void GameServer::handleDisconnections()
{ 
	for (auto itr = mPeers.begin(); itr != mPeers.end();)
	{
		PeerPtr& peer = *itr;
		if (peer->timedOut)
		{
			for (int32_t aircraftIdentifier : peer->aircraftIdentifiers)
			{
				sf::Packet packet;
				packet << static_cast<int32_t> (Server::PlayerDisconnect)
					<< aircraftIdentifier;

				sendToAll(packet);

				mAircraftInfo.erase(aircraftIdentifier);
			}

			if (mConnectedPlayers == mMaxConnectedPlayers)
			{
				setListening(true);
			}

			--mConnectedPlayers;
			mAircraftCount -= peer->aircraftIdentifiers.size();

			itr = mPeers.erase(itr);

			broadcastMessage("An ally has disconnected.");
		}
		else
		{
			++itr;
		}
	}
}

void GameServer::informWorldState(sf::TcpSocket& socket)
{  
	sf::Packet packet;
	packet << static_cast<int32_t> (Server::InitialState);
	// Sends the how total distance and the bottom y of the page
	packet << mWorldHeight << mBattleFieldRect.position.y + mBattleFieldRect.size.y;
	packet << static_cast<int32_t> (mAircraftCount);

	for (PeerPtr& peer : mPeers)
	{
		if (peer->ready)
		{
			for (int32_t identifier : peer->aircraftIdentifiers)
			{
				packet << identifier;
				packet << mAircraftInfo[identifier].position.x
					<< mAircraftInfo[identifier].position.y
					<< mAircraftInfo[identifier].hitpoints
					<< mAircraftInfo[identifier].missileAmmo;
			}
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{ 
	sf::Packet packet;
	packet << static_cast<int32_t> (Server::BroadcastMessage);
	packet << message;
	
	sendToAll(packet);
}

void GameServer::sendToAll(sf::Packet& packet)
{ 
	for (PeerPtr& peer : mPeers)
	{
		if (peer->ready)
		{
			peer->socket.send(packet);
		}
	}
}

void GameServer::updateClientState()
{
	sf::Packet packet;
	packet << static_cast<int32_t> (Server::UpdateClientState)
		<< static_cast<float> (mBattleFieldRect.position.y + mBattleFieldRect.size.y)
		<< static_cast<int32_t> (mAircraftInfo.size());

	for (auto pair : mAircraftInfo)
	{
		packet << pair.first << pair.second.position.x << pair.second.position.y;
	}

	sendToAll(packet);
}