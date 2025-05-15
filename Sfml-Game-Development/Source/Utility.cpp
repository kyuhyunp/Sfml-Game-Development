#include "../Header/Utility.hpp"

void centerOrigin(sf::Sprite& sprite)
{
	sf::Vector2f center = sprite.getLocalBounds().getCenter();
	sprite.setOrigin({ std::floor(center.x), std::floor(center.y) });
}

void centerOrigin(sf::Text& text)
{
	sf::Vector2f center = text.getLocalBounds().getCenter();
	text.setOrigin({ std::floor(center.x), std::floor(center.y) });
}