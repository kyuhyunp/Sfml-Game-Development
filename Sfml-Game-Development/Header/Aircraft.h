#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"
#include "ResourceIdentifiers.h"
#include "TextNode.h"
#include "Projectile.h"
#include "Command.h"
#include "Animation.h"


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
	virtual sf::FloatRect getBoundingRect() const;
	virtual void remove();
	virtual bool isMarkedForRemoval() const;
	bool isAllied() const;
	float getMaxSpeed() const;

	void increaseSpread();
	void increaseFireRate();
	void collectMissiles(unsigned int count);

	void fire();
	void launchMissile();

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);

	void updateMovementPattern(sf::Time dt);
	
	void checkPickupDrop(CommandQueue& commands);
	void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, Projectile::Type type, 
		float xOffset, float yOffset, const TextureHolder& textures) const;
	void createPickup(SceneNode& node, const TextureHolder& textures) const;

	void updateTexts();
	void updateRollAnimation();

	Type mType;
	sf::Sprite mSprite;
	Animation mExplosion;
	Command mFireCommand;
	Command mMissileCommand;
	Command mDropPickupCommand;
	sf::Time mFireCountdown;
	bool mIsFiring;
	bool mIsLaunchingMissile;
	bool mShowExplosion;
	bool mSpawnedPickup;

	int mFireRateLevel;
	int mSpreadLevel;
	int mMissileAmmo;

	float mTravelledDistance;
	size_t mDirectionIndex;
	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;

	
};

#endif

