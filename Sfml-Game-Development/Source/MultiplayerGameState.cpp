#include "../Header/MultiplayerGameState.h"
#include "../Header/MusicPlayer.h"
#include "../Header/Utility.hpp"

#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include <iostream>


sf::IpAddress getAddressFromFile()
{
	{ // RAII block to open existing file
		std::ifstream inputFile("ip.txt");
		std::string ipAddress;
		if (inputFile >> ipAddress)
		{
			auto address = sf::IpAddress::resolve(ipAddress);
			if (address.has_value())
			{
				return address.value();
			}
		}
	}

	std::ofstream outputFile("ip.txt");
	std::string localAddress = "127.0.0.1";
	outputFile << localAddress;
	auto address = sf::IpAddress::resolve(localAddress);
	return address.value();
}

sf::IpAddress toIpAddress(const std::string& address)
{
	auto ipAddress = sf::IpAddress::resolve(address);
	if (ipAddress.has_value())
	{
		return ipAddress.value();
	}
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool isHost)
	: State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds, true) 
	, mWindow(*context.window)
	, mTextureHolder(*context.textures)
	, mPlayers()
	, mLocalPlayerIdentifiers()
	, mSocket()
	, mConnected(false)
	, mGameServer(nullptr)
	, mTickClock()
	, mBroadcasts()
	, mBroadcastText(context.fonts->get(Fonts::ID::Sansation))
	, mBroadcastElapsedTime(sf::Time::Zero)
	, mPlayerInvitationText(context.fonts->get(Fonts::ID::Sansation),
		"Press Enter to spawn player 2", 20)
	, mPlayerInvitationTime(sf::Time::Zero)
	, mFailedConnectionText(context.fonts->get(Fonts::ID::Sansation),
		"Attempting to connect...", 35)
	, mFailedConnectionClock()
	, mActiveState(true)
	, mHasFocus(true)
	, mHost(isHost)
	, mGameStarted(false)
	, mClientTimeout(sf::seconds(2.f))
	, mTimeSinceLastPacket(sf::seconds(0.f))
{
	mBroadcastText.setPosition({ 1024.f / 2, 100.f });

	mPlayerInvitationText.setFillColor(sf::Color::White);
	mPlayerInvitationText.setPosition({ 1000 - mPlayerInvitationText.getLocalBounds().size.x,
		700 - mPlayerInvitationText.getLocalBounds().size.y });

	mFailedConnectionText.setFillColor(sf::Color::White);
	centerOrigin(mFailedConnectionText);
	mFailedConnectionText.setPosition({ mWindow.getSize().x / 2.f , 
		mWindow.getSize().y / 2.f });

	mWindow.clear(sf::Color::Black);
	mWindow.draw(mFailedConnectionText);
	mWindow.display();

	mFailedConnectionText.setString("Could not connect to the remote server!");
	centerOrigin(mFailedConnectionText);

	sf::IpAddress ip (127, 0, 0, 1);
	if (isHost)
	{
		mGameServer.reset(new GameServer(
			sf::Vector2f({ static_cast<float> (mWindow.getSize().x)
			, static_cast<float> (mWindow.getSize().y) })));
	}
	else
	{
		ip = getAddressFromFile();
	}

	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Status::Done)
	{
		mConnected = true;
	}
	else
	{
		mFailedConnectionClock.restart();
	}

	mSocket.setBlocking(false);

	context.music->play(Music::ID::MissionTheme);
}

void MultiplayerGameState::draw()
{ 
	if (mConnected)
	{
		mWorld.draw();

		mWindow.setView(mWindow.getDefaultView());
		if (!mBroadcasts.empty())
		{
			mWindow.draw(mBroadcastText);
		}
		if (mLocalPlayerIdentifiers.size() < 2 && mPlayerInvitationTime < sf::seconds(0.5f))
		{
			mWindow.draw(mPlayerInvitationText);
		}
	}
	else
	{
		mWindow.draw(mFailedConnectionText);
	}
}

bool MultiplayerGameState::update(sf::Time dt)
{ 
	if (mConnected)
	{
		mWorld.update(dt);

		bool foundLocalPlane = false;
		for (auto itr = mPlayers.begin(); itr != mPlayers.end();)
		{
			if (!mWorld.getAircraft(itr->first))
			{
				itr = mPlayers.erase(itr);

				if (mPlayers.empty())
				{ // No players left
					requestStackPush(States::GameOver);
				}
			}
			else
			{
				if (std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(),
					itr->first) != mLocalPlayerIdentifiers.end())
				{ // Local players are never removed
					foundLocalPlane = true;
				}

				++itr;
			}
		}

		if (!foundLocalPlane && mGameStarted)
		{ // No local player left
			requestStackPush(States::GameOver);
		}

		// Only handle local realtime input if the player is playing
		CommandQueue& commands = mWorld.getCommandQueue();
		if (mActiveState && mHasFocus)
		{
			for (auto& pair : mPlayers)
			{
				pair.second->handleRealtimeInput(commands);
			}
		}

		// Always handle remote realtime input
		for (auto& pair : mPlayers)
		{
			pair.second->handleRealtimeNetworkInput(commands);
		}

		sf::Packet packet;
		if (mSocket.receive(packet) == sf::TcpSocket::Status::Done)
		{ 
			mTimeSinceLastPacket = sf::Time::Zero;

			int32_t packetType;
			packet >> packetType;
			handlePacket(packetType, packet);
		}
		else if (mClientTimeout < mTimeSinceLastPacket)
		{ // No retry happens
			mConnected = false;

			mFailedConnectionText.setString("Lost connection to server");
			centerOrigin(mFailedConnectionText);

			mFailedConnectionClock.restart();
		}

		updateBroadcastMessage(dt);

		// Preventing overflow for player invitation text that blinks every half second 
		mPlayerInvitationTime += dt;
		if (mPlayerInvitationTime > sf::seconds(1.f))
		{
			mPlayerInvitationTime = sf::Time::Zero;
		}

		// Destroyed an enemy
		GameActions::Action gameAction;
		while (mWorld.pollGameAction(gameAction))
		{
			sf::Packet packet;
			packet << static_cast<int32_t> (Client::GameEvent);
			packet << static_cast<int32_t> (gameAction.type);
			packet << gameAction.position.x << gameAction.position.y;

			mSocket.send(packet);
		}

		// Sending constant position update to the server
		if (mTickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<int32_t> (Client::PositionUpdate);
			positionUpdatePacket << static_cast<int32_t> (mLocalPlayerIdentifiers.size());
			for (int32_t identifier : mLocalPlayerIdentifiers)
			{
				if (Aircraft* aircraft = mWorld.getAircraft(identifier))
				{
					positionUpdatePacket << identifier
						<< aircraft->getPosition().x << aircraft->getPosition().y
						<< static_cast<int32_t> (aircraft->getHitpoints())
						<< static_cast<int32_t> (aircraft->getMissileAmmo());
				}
			} 

			mSocket.send(positionUpdatePacket);

			mTickClock.restart();
		}

		mTimeSinceLastPacket += dt;
	}
	else if (sf::seconds(5.f) < mFailedConnectionClock.getElapsedTime())
	{
		requestStateClear();
		requestStackPush(States::Menu);
	}

	return true;
}

bool MultiplayerGameState::handleEvent(const sf::Event& event)
{ // Both realtime network change and event relies on handleEvent
	CommandQueue commands = mWorld.getCommandQueue();

	for (auto& pair : mPlayers)
	{ // Player's handleEvent only processes local events
		pair.second->handleEvent(event, commands);
	}

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Enter
			&& mLocalPlayerIdentifiers.size() == 1)
		{
			sf::Packet packet;
			packet << static_cast<int32_t> (Client::RequestCoopPartner);

			mSocket.send(packet);
		}

		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{
			// Real-time actions are disabled even if keys are pressed
			disableAllRealtimeActions();
			requestStackPush(States::NetworkPause);
		}
	} 
	else if (event.is<sf::Event::FocusGained>())
	{
		mHasFocus = true;
	}
	else if (event.is<sf::Event::FocusLost>())
	{
		mHasFocus = false;
	}

	return true;
}

void MultiplayerGameState::onActivate()
{ // StateStack's applyPendingChanges calls it after NetworkPause
	mActiveState = true;
}

void MultiplayerGameState::onDestroy()
{ // Used in StateStack's applyPendingChanges
	if (!mHost && mConnected)
	{
		sf::Packet packet;
		packet << static_cast<int32_t> (Client::Quit);

		mSocket.send(packet);
	}
}

void MultiplayerGameState::disableAllRealtimeActions()
{ 
	mActiveState = false;

	for (int32_t localIdentifier : mLocalPlayerIdentifiers)
	{
		mPlayers[localIdentifier]->disableAllRealtimeActions();
	}
}

void MultiplayerGameState::updateBroadcastMessage(sf::Time elapsedTime)
{ 
	if (mBroadcasts.empty())
	{
		return;
	}

	mBroadcastElapsedTime += elapsedTime;

	if (mBroadcastElapsedTime > sf::seconds(2.5f))
	{
		mBroadcasts.erase(mBroadcasts.begin());

		std::string next;
		if (!mBroadcasts.empty())
		{
			next = mBroadcasts.front();
			mBroadcastElapsedTime = sf::Time::Zero;
		}

		mBroadcastText.setString(next);
		centerOrigin(mBroadcastText);
	}
}

void MultiplayerGameState::handlePacket(int32_t packetType, sf::Packet& packet)
{
	switch (packetType)
	{
	case Server::BroadcastMessage:
	{
		std::string message;
		packet >> message;
		mBroadcasts.push_back(message);

		if (mBroadcasts.size() == 1)
		{
			mBroadcastText.setString(mBroadcasts.front());
			centerOrigin(mBroadcastText);
			mBroadcastElapsedTime = sf::Time::Zero;
		}

		std::cout << "Client Received Server::BroadcastMessage\n";
	} break;

	case Server::SpawnSelf:
	{
		int32_t aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);

		mPlayers[aircraftIdentifier].reset(
			new Player(&mSocket, aircraftIdentifier, getContext().keys1));
		mLocalPlayerIdentifiers.push_back(aircraftIdentifier);

		mGameStarted = true;

		std::cout << "Client Received Server::SpawnSelf\n";
	} break;

	case Server::PlayerConnect:
	{	
		int32_t aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);

		// Remote player doesn't send packet, socket is redundant
		mPlayers[aircraftIdentifier].reset(
			new Player(nullptr, aircraftIdentifier, nullptr));

		std::cout << "Client Received Server::PlayerConnect\n";
	}	break;

	case Server::PlayerDisconnect:
	{	
		int32_t aircraftIdentifier;
		packet >> aircraftIdentifier;

		mWorld.removeAircraft(aircraftIdentifier);
		mPlayers.erase(aircraftIdentifier);

		std::cout << "Client Received Server::PlayerDisconnect\n";
	}	break;

	case Server::InitialState:
	{ 
		float worldHeight;
		float initialScrolling;
		packet >> worldHeight >> initialScrolling;
		mWorld.setWorldHeight(worldHeight);
		mWorld.setCurrentBattleFieldPosition(initialScrolling);

		int32_t aircraftCount;
		packet >> aircraftCount;
		for (int32_t i = 0; i < aircraftCount; ++i)
		{ // Setting remote aircrafts
			int32_t aircraftIdentifier;
			sf::Vector2f aircraftPosition;
			int32_t aircraftHitpoints = 0;
			int32_t aircraftMissileAmmo = 0;

			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y
				>> aircraftHitpoints >> aircraftMissileAmmo;

			Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
			aircraft->setPosition(aircraftPosition);
			aircraft->setHitpoints(aircraftHitpoints);
			aircraft->setMissileAmmo(aircraftMissileAmmo);

			mPlayers[aircraftIdentifier].reset(
				new Player(&mSocket, aircraftIdentifier, nullptr));
		}

		std::cout << "Client Received Server::InitialState\n";
	}	break;

	case Server::PlayerEvent:
	{	
		int32_t identifier;
		int32_t action;
		packet >> identifier >> action;

		CommandQueue& commands = mWorld.getCommandQueue();
		auto itr = mPlayers.find(identifier);
		if (itr != mPlayers.end())
		{
			itr->second->handleNetworkEvent(
				static_cast<Player::Action> (action), commands);
		}

		std::cout << "Client Received Server::PlayerEvent\n";
	}	break;

	case Server::PlayerRealtimeChange:
	{	
		int32_t aircraftIdentifier;
		int32_t action;
		bool actionEnabled;

		packet >> aircraftIdentifier >> action >> actionEnabled;

		auto itr = mPlayers.find(aircraftIdentifier);
		if (itr != mPlayers.end())
		{
			itr->second->handleNetworkRealtimeChange(static_cast<Player::Action> (action),
				actionEnabled);
		}

		std::cout << "Client Received Server::PlayerRealtimeChange\n";
	}	break;

	case Server::AcceptCoopPartner:
	{	
		int32_t aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);

		mPlayers[aircraftIdentifier].reset(
			new Player(&mSocket, aircraftIdentifier, getContext().keys2));
		mLocalPlayerIdentifiers.push_back(aircraftIdentifier);

		std::cout << "Client Received Server::AcceptCoopPartner\n";
	}	break;

	case Server::SpawnEnemy:
	{	
		int32_t type;
		float height;
		float relativeX;
		packet >> type >> height >> relativeX;

		mWorld.addEnemy(static_cast<Aircraft::Type> (type), relativeX, height);
		mWorld.sortEnemies();

		std::cout << "Client Received Server::SpawnEnemy\n";
	}	break;

	case Server::MissionSuccess:
	{
		requestStackPush(States::MissionSuccess);

		std::cout << "Client Received Server::MissionSuccess\n";
	}	break;

	case Server::SpawnPickup:
	{	
		int32_t type;
		sf::Vector2f position;
		packet >> type >> position.x >> position.y;

		mWorld.createPickup(position, static_cast<Pickup::Type> (type));

		std::cout << "Client Received Server::SpawnPickup\n";
	}	break;

	case Server::UpdateClientState:
	{	
		float currentWorldPosition;
		int32_t aircraftCount;
		packet >> currentWorldPosition >> aircraftCount;

		float currentViewPosition = 
			mWorld.getViewBounds().position.y + mWorld.getViewBounds().size.y;

		/*
		* If view > world, 
		*	1.f + 0.001f * (currentViewPosition - currentWorldPosition) > 1
		* If world > view,
		*	1.f + 0.001f * (currentViewPosition - currentWorldPosition) < 1
		*/
		mWorld.setWorldScrollCompensation(
			1.f + 0.001f * (currentViewPosition - currentWorldPosition));
		
		for (int32_t i = 0; i < aircraftCount; ++i)
		{
			int32_t identifier;
			sf::Vector2f position;
			packet >> identifier >> position.x >> position.y;
			
			Aircraft* aircraft = mWorld.getAircraft(identifier);
			bool isLocalPlane = std::find(mLocalPlayerIdentifiers.begin(),
				mLocalPlayerIdentifiers.end(), identifier) != mLocalPlayerIdentifiers.end();
			if (aircraft && !isLocalPlane)
			{
				sf::Vector2f interpolatedPosition = aircraft->getPosition() + 
					(position - aircraft->getPosition()) * 0.1f;
				aircraft->setPosition(position);
			}
		}
	}	break;
	
	default:
	{
	}	break;
	}
}

