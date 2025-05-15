#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
#include <sstream>


namespace sf
{
	class Sprite;
	class Text;
}

// Since std::to_string doesn't work on MinGW we have to implement
// our own to support all platforms.
template <typename T>
std::string toString(const T& value);

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);

#include "Utility.inl"
#endif // BOOK_STRINGHELPERS_HPP
