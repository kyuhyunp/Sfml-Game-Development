#include "../Header/DataTables.h"
#include "../Header/Aircraft.h"
#include "../Header/Projectile.h"


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