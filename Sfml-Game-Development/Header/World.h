#ifndef WORLD_H
#define WORLD_H


#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"
#include "Aircraft.h"
#include "SpriteNode.h"
#include "SceneNode.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>

namespace sf
{
	class RenderWindow;
}

class World
{
public:
	explicit World(sf::RenderWindow& window);

	// Non copyable
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	void update(sf::Time dt);
	void draw();

private:
	void loadTextures();
	void buildScene();

	enum Layer {
		Background,
		Air,
		LayerCount
	};

	sf::RenderWindow& mWindow;
	sf::View mWorldView;
	TextureHolder mTextures;
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
};

#endif
