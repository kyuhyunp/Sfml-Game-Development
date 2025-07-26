#ifndef RESOURCEIDENTIFIERS_H
#define RESOURCEIDENTIFIERS_H

// Forward declaration
namespace sf
{
	class Font;
	class Texture;
}


namespace Textures
{
	enum class ID : int
	{
		Eagle,
		Raptor,
		Avenger,
		Bullet,
		Missile,
		Desert,
		HealthRefill,
		MissileRefill,
		FireSpread,
		FireRate,
		TitleScreen,
		ButtonNormal,
		ButtonSelected,
		ButtonPressed,
	};
}

namespace Fonts
{
	enum class ID : int
	{
		Sansation
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;


#endif