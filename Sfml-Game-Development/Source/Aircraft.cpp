#include "../Header/Aircraft.h"

Textures::ID toTextureID(Aircraft::Type type)
{
	switch (type) 
	{
	case Aircraft::Type::Eagle:
		return Textures::ID::Eagle;

	case Aircraft::Type::Raptor:
		return Textures::ID::Raptor;
	}

	return Textures::ID::Eagle;
}

Aircraft::Aircraft(Type type, const TextureHolder& textures): 
	mType(type), mSprite(textures.get(toTextureID(type))) 
{
	sf::FloatRect origin = mSprite.getLocalBounds();
	mSprite.setOrigin(origin.getCenter());
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
