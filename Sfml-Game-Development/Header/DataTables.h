#ifndef DATA_TABLES_H
#define DATA_TABLES_H

#include "ResourceIdentifiers.h"

#include <SFML/System/Time.hpp>
#include <functional>
#include <vector>

class Aircraft;

struct Direction
{
	Direction(float angle, float distance)
		: angle(angle)
		, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct AircraftData
{	
	int hitpoints;
	float speed;
	Textures::ID texture;
	sf::Time fireInterval;
	std::vector<Direction> directions;
};

struct ProjectileData
{
	int damage;
	float speed;
	Textures::ID texture;
};

struct PickupData
{
	std::function<void(Aircraft&)> action;
	Textures::ID texture;
};

std::vector<AircraftData> initializeAircraftData();
std::vector<ProjectileData> initializeProjectileData();
std::vector<PickupData> initializePickupData();

#endif
