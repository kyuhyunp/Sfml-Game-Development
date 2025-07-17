#include "../Header/Aircraft.h"
#include "../Header/DataTables.h"
#include "../Header/Utility.hpp"


namespace
{
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Textures::ID toTextureID(Aircraft::Type type)
{
	switch (type) 
	{
	case Aircraft::Eagle:
		return Textures::ID::Eagle;

	case Aircraft::Raptor:
		return Textures::ID::Raptor;

	case Aircraft::Avenger:
		return Textures::ID::Avenger;
	}

	return Textures::ID::Eagle;
}

Aircraft::Aircraft(Type type, const FontHolder& fonts, const TextureHolder& textures)
	: Entity(Table[type].hitpoints)
	, mType(type) 
	, mSprite(textures.get(toTextureID(type)))
	, mFireCommand()
	, mMissileCommand()
	, mFireCountdown(sf::Time::Zero)
	, mIsFiring(false)
	, mIsLaunchingMissile(false)
	, mFireRateLevel(1)
	, mSpreadLevel(1)
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
	, mHealthDisplay()
{
	centerOrigin(mSprite);

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, "HP 100"));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	mFireCommand.category = Category::SceneAirLayer;
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time dt)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::SceneAirLayer;
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time dt)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{
	target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	checkProjectileLaunch(dt, commands);

	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);

	updateTexts();
}

unsigned int Aircraft::getCategory() const 
{
	switch (mType)
	{
	case Eagle:
		return Category::PlayerAircraft;
	default:
		return Category::EnemyAircraft;
	}
}

bool Aircraft::isAllied() const
{
	return mType == Eagle;
}

float Aircraft::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Aircraft::fire()
{
	mIsFiring = true;
}

void Aircraft::launchMissile()
{
	mIsLaunchingMissile = true;
}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	const std::vector<Direction>& directions = Table[mType].directions;

	if (!directions.empty())
	{
		float distanceToTravel = directions[mDirectionIndex].distance;
		if (mTravelledDistance > distanceToTravel)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Facing downward -> rotated 90 deg counterclockwise
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);
		setVelocity(vx, vy);
		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	if (!isAllied())
	{
		fire();
	}
	
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		commands.push(mFireCommand);

		assert(mFireRateLevel > 0);
		mFireCountdown += sf::seconds(1.f / (mFireRateLevel + 1));
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		mFireCountdown -= dt;
		if (mFireCountdown < sf::Time::Zero)
		{
			mFireCountdown = sf::Time::Zero;
		}
		/*
		* Without the below line, there is a chance of another bullet firing
		* because there is a chance of space pressed for the next dt
		*/
		mIsFiring = false;
	}



	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet 
		: Projectile::EnemyBullet;

	switch (mSpreadLevel)
	{
	case 1: 
		createProjectile(node, type, 0.f, 0.5f, textures);
		break;
	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, +0.33f, 0.33f, textures);
		break;
	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.f, 0.5f, textures);
		createProjectile(node, type, +0.5f, 0.33f, textures);
		break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type,
	float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(
		xOffset * mSprite.getGlobalBounds().size.x,
		yOffset * mSprite.getGlobalBounds().size.y);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = (type == Projectile::AlliedBullet) ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::updateTexts()
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition({ 0.f, 50.f });

	// Set rotation back to original
	mHealthDisplay->setRotation(-getRotation()); 
}
