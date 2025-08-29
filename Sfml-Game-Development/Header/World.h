#ifndef WORLD_H
#define WORLD_H

#include "ResourceIdentifiers.h"
#include "ResourceHolder.hpp"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "CommandQueue.h"
#include "Aircraft.h"
#include "Category.h"
#include "BloomEffect.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <algorithm>

namespace sf
{
	class RenderTarget;
}


class World
{
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts);

	// Non copyable
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	void update(sf::Time dt);
	void draw();

	CommandQueue& getCommandQueue();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;

private:
	void loadTextures();
	void adaptPlayerVelocity();
	void adaptPlayerPosition();
	void handleCollisions();

	void buildScene();
	void addEnemies();
	void addEnemy(Aircraft::Type type, float relX, float relY);
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();
	sf::FloatRect getViewBounds() const;
	sf::FloatRect getBattlefieldBounds() const;


	enum Layer 
	{
		Background,
		LowerAir,
		UpperAir,
		LayerCount
	};

	struct SpawnPoint
	{
		SpawnPoint(Aircraft::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Aircraft::Type type;
		float x;
		float y;
	};

	sf::RenderTarget& mTarget;
	sf::RenderTexture mSceneTexture;
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

	std::vector<SpawnPoint> mEnemySpawnPoints;
	std::vector<Aircraft*> mActiveEnemies;

	BloomEffect mBloomEffect;
};

#endif
