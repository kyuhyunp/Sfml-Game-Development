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
	, mHealthDisplay()
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
{
	centerOrigin(mSprite);

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, "HP 100"));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{
	target.draw(mSprite, states);
}

unsigned int Aircraft::getCategory() const {
	switch (mType)
	{
	case Type::Eagle:
		return Category::PlayerAircraft;
	default:
		return Category::EnemyAircraft;
	}
}

float Aircraft::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Aircraft::updateCurrent(sf::Time dt)
{
	updateMovementPattern(dt);
	Entity::updateCurrent(dt);
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

void Aircraft::updateTexts()
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition({ 0.f, 50.f });
	mHealthDisplay->setRotation(-getRotation());
}
