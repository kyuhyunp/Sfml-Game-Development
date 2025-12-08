#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>


const unsigned short ServerPort = 5001;

namespace Server
{ // Packets originated in the server beginning with int32_t packetType
	enum PacketType
	{
		BroadcastMessage,
		SpawnSelf,
		InitialState,
		PlayerEvent,
		PlayerRealtimeChange,
		PlayerConnect,
		PlayerDisconnect,
		AcceptCoopPartner,
		SpawnEnemy,
		SpawnPickup,
		UpdateClientState,
		MissionSuccess
	};
}

namespace Client
{ // Packets originated in the client beginning with int32_t packetType
	enum PacketType
	{
		PlayerEvent,
		PlayerRealtimeChange,
		RequestCoopPartner,
		PositionUpdate,
		GameEvent,
		Quit
	};
}

namespace PlayerActions
{
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
}

namespace GameActions
{
	enum Type
	{
		EnemyExplode,
	};

	struct Action
	{
		Action()
		{ 
		}

		Action(Type type, sf::Vector2f position)
			: type(type)
			, position(position)
		{
		}

		Type type;
		sf::Vector2f position;
	};
}

#endif