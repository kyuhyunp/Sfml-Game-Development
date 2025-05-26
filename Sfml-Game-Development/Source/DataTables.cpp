#include "../Header/DataTables.h"


std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(Aircraft::TypeCount);

	data[Aircraft::Eagle].hitpoints = 100;
	data[Aircraft::Eagle].speed = 200.f;
	data[Aircraft::Eagle].texture = Textures::ID::Eagle;

	data[Aircraft::Raptor].hitpoints = 20;
	data[Aircraft::Raptor].speed = 80.f;
	data[Aircraft::Raptor].texture = Textures::ID::Raptor;
	data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
	data[Aircraft::Raptor].directions.emplace_back(-45.f, 160.f);
	data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);

	data[Aircraft::Avenger].hitpoints = 40;
	data[Aircraft::Avenger].speed = 50.f;
	data[Aircraft::Avenger].texture = Textures::ID::Avenger;
	data[Aircraft::Avenger].directions.emplace_back(+45.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(-45.f, 100.f);
	data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(+45.f, 50.f);

	return data;
}