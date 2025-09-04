#ifndef RESOURCEIDENTIFIERS_H
#define RESOURCEIDENTIFIERS_H

// Forward declaration
namespace sf
{
	class Font;
	class Texture;
	class Shader;
}


namespace Textures
{
	enum class ID : int
	{
		Entities,
		Jungle,
		TitleScreen,
		Buttons,
		Explosion,
		Particle,
		FinishLine,
	};
}

namespace Shaders
{
	enum class ID : int
	{
		BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass,
	};
}

namespace Fonts
{
	enum class ID : int
	{
		Sansation
	};
}

namespace Music
{
	enum class ID : int
	{
		MenuTheme,
		MissionTheme
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID> ShaderHolder;


#endif