#ifndef WORLD_H
#define WORLD_H

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"
#include "Aircraft.h"
#include "SpriteNode.h"
#include "SceneNode.h"
#include "CommandQueue.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <algorithm>

namespace sf
{
	class RenderWindow;
}

class World
{
public:
	explicit World(sf::RenderWindow& window, FontHolder& fonts);

	// Non copyable
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	void update(sf::Time dt);
	void draw();

	CommandQueue& getCommandQueue();

private:
	void loadTextures();
	void buildScene();
	void adaptPlayerVelocity();
	void adaptPlayerPosition();

	void addEnemies();

	enum Layer {
		Background,
		Air,
		LayerCount
	};

	sf::RenderWindow& mWindow;
	sf::View mWorldView;
	TextureHolder mTextures;
	FontHolder& mFonts;

	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	CommandQueue mCommandQueue;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	Aircraft* mRaptor;
	Aircraft* mAvenger;
};

#endif
