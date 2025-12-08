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
#include "SoundPlayer.h"
#include "NetworkProtocol.h"
#include "Pickup.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <algorithm>

namespace sf
{
	class RenderTarget;
}

class NetworkNode;

class World
{
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, 
		bool networked);

	// Non copyable
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	void update(sf::Time dt);
	void draw();


	sf::FloatRect getViewBounds() const;
	CommandQueue& getCommandQueue();
	Aircraft* addAircraft(int32_t identifier);
	void removeAircraft(int32_t identifier);
	void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);

	void addEnemy(Aircraft::Type type, float relX, float relY);
	void sortEnemies();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;

	void setWorldScrollCompensation(float compensation);

	Aircraft* getAircraft(int32_t identifier) const;
	sf::FloatRect getBattlefieldBounds() const;

	void createPickup(sf::Vector2f position, Pickup::Type type);
	bool pollGameAction(GameActions::Action& out);

private:
	void loadTextures();
	void adaptPlayerVelocity();
	void adaptPlayerPosition();
	void handleCollisions();
	void updateSounds();

	void buildScene();
	void addEnemies();
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();


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
	SoundPlayer& mSounds;

	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	CommandQueue mCommandQueue;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	float mScrollSpeedCompensation;
	std::vector<Aircraft*> mPlayerAircrafts;

	std::deque<SpawnPoint> mEnemySpawnPoints;
	std::vector<Aircraft*> mActiveEnemies;

	BloomEffect mBloomEffect;

	bool mNetworkedWorld;
	NetworkNode* mNetworkNode;
};

#endif
