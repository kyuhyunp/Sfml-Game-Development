#include "../Header/Projectile.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/DataTables.h"
#include "../Header/Utility.hpp"


namespace
{
	const std::vector<ProjectileData> Table = initializeProjectileData();
}

Projectile::Projectile(Type type, const TextureHolder& textures)
	: Entity(1)
	, mType(type)
	, mSprite(textures.get(Table[type].texture))
	, mTargetDirection()
{
	centerOrigin(mSprite);
}

void Projectile::guideTowards(sf::Vector2f position)
{
	assert(isGuided());
	mTargetDirection = (position - getWorldPosition()).normalized();
}

bool Projectile::isGuided() const
{
	return mType == Missile;
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (isGuided())
	{ // Steering behavior (evasion, interception, group behavior)
		const float approachRate = 200.f;

		sf::Vector2f newVelocity = (approachRate * dt.asSeconds() * 
			mTargetDirection + getVelocity()).normalized();
		
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);

		// missile is facing up so rotate counter clockwise by 90 degrees
		setRotation(sf::degrees(toDegree(angle) + 90.f));
		setVelocity(newVelocity);
	}

	Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

unsigned int Projectile::getCategory() const
{
	return mType == EnemyBullet ? Category::EnemyProjectile : Category::AlliedProjectile;
}

sf::FloatRect Projectile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const
{
	return Table[mType].speed;
}

int Projectile::getDamage() const
{
	return Table[mType].damage;
}