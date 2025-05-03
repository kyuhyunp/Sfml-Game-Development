#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Entity.h"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"

class Aircraft : public Entity
{
public:
	enum class Type : int {
		Eagle, 
		Raptor
	};

	explicit Aircraft(Type type, const ResourceHolder<sf::Texture, Textures::ID>& textures);

	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type mType;
	sf::Sprite mSprite;
};

#endif

