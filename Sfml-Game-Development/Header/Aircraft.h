#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"
#include "TextNode.h"
#include "Projectile.h"


class Aircraft : public Entity
{
public:
	enum Type 
	{
		Eagle, 
		Raptor,
		Avenger,
		TypeCount
	};

	explicit Aircraft(Type type, const FontHolder& fonts, const TextureHolder& textures);

	virtual unsigned int getCategory() const;
	bool isAllied() const;
	float getMaxSpeed() const;

	void fire();
	void launchMissile();

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);

	void updateMovementPattern(sf::Time dt);
	
	void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, Projectile::Type type, 
		float xOffset, float yOffset, const TextureHolder& textures) const;

	void updateTexts();

	Type mType;
	sf::Sprite mSprite;
	Command mFireCommand;
	Command mMissileCommand;
	sf::Time mFireCountdown;
	bool mIsFiring;
	bool mIsLaunchingMissile;

	int mFireRateLevel;
	int mSpreadLevel;

	float mTravelledDistance;
	size_t mDirectionIndex;
	TextNode* mHealthDisplay;

	int counter;
};

#endif

