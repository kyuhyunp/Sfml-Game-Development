#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"
#include "TextNode.h"


class Aircraft : public Entity
{
public:
	enum Type 
	{
		Eagle, 
		Raptor,
		Avenger,
		TypeCount,
	};

	explicit Aircraft(Type type, const FontHolder& fonts, const TextureHolder& textures);

	virtual unsigned int getCategory() const;

	float getMaxSpeed() const;

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt);

	void updateMovementPattern(sf::Time dt);

	void updateTexts();

	Type mType;
	sf::Sprite mSprite;
	TextNode* mHealthDisplay;

	float mTravelledDistance;
	size_t mDirectionIndex;
};

#endif

