#include "../Header/DataTables.h"
#include "../Header/Aircraft.h"
#include "../Header/Projectile.h"
#include "../Header/Pickup.h"


using namespace std::placeholders;

std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(Aircraft::TypeCount);

	data[Aircraft::Eagle].hitpoints = 100;
	data[Aircraft::Eagle].speed = 200.f;
	data[Aircraft::Eagle].texture = Textures::ID::Eagle;
	data[Aircraft::Eagle].fireInterval = sf::seconds(1);

	data[Aircraft::Raptor].hitpoints = 20;
	data[Aircraft::Raptor].speed = 80.f;
	data[Aircraft::Raptor].texture = Textures::ID::Raptor;
	data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
	data[Aircraft::Raptor].directions.emplace_back(-45.f, 160.f);
	data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
	data[Aircraft::Raptor].fireInterval = sf::Time::Zero;

	data[Aircraft::Avenger].hitpoints = 40;
	data[Aircraft::Avenger].speed = 50.f;
	data[Aircraft::Avenger].texture = Textures::ID::Avenger;
	data[Aircraft::Avenger].directions.emplace_back(+45.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(-45.f, 100.f);
	data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
	data[Aircraft::Avenger].directions.emplace_back(+45.f, 50.f);
	data[Aircraft::Avenger].fireInterval = sf::seconds(2);
	

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
	data[Projectile::AlliedBullet].texture = Textures::ID::Bullet;

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::ID::Bullet;

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::ID::Missile;

	return data;
}

std::vector<PickupData> initializePickupData()
{ // placeholder _1 is used for bind for which Aircraft the action is taken
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::ID::HealthRefill;
	data[Pickup::HealthRefill].action = std::bind(&Aircraft::repair, _1, 25);

	data[Pickup::MissileRefill].texture = Textures::ID::MissileRefill;
	data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);

	data[Pickup::FireSpread].texture = Textures::ID::FireSpread;
	data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);

	data[Pickup::FireRate].texture = Textures::ID::FireRate;
	data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);

	return data;
}