#include "../Header/World.h"
#include "../Header/Pickup.h"
#include "../Header/SceneNode.h"
#include "../Header/Particle.h"
#include "../Header/ParticleNode.h"


World::World(sf::RenderWindow& window, FontHolder& fonts)
	: mWindow(window)
	, mWorldView(window.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSceneGraph()
	, mSceneLayers()
	, mCommandQueue()
	, mWorldBounds({ 0.f, 0.f }, { mWorldView.getSize().x, 2000.f })
	, mSpawnPosition(mWorldView.getSize().x / 2.f, 
		mWorldBounds.size.y - mWorldView.getSize().y / 2.f)
	, mScrollSpeed(-50.f)
	, mPlayerAircraft(nullptr)
	, mEnemySpawnPoints()
{
	loadTextures();
	buildScene();

	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	mWorldView.move({ 0.f, mScrollSpeed * dt.asSeconds()});
	mPlayerAircraft->setVelocity({ 0.f, 0.f });

	destroyEntitiesOutsideView();
	guideMissiles();

	while (!mCommandQueue.isEmpty())
	{
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	}

	adaptPlayerVelocity();

	handleCollisions();
	mSceneGraph.removeWrecks();

	spawnEnemies();

	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
}

void World::draw()
{ 
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return !mPlayerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !mWorldBounds.contains(mPlayerAircraft->getPosition());
}

void World::loadTextures() 
{
	mTextures.load(Textures::ID::Entities, "Media/Textures/Entities.png");
	mTextures.load(Textures::ID::Jungle, "Media/Textures/Jungle.png");
	mTextures.load(Textures::ID::Particle, "Media/Textures/Particle.png");
	mTextures.load(Textures::ID::Explosion, "Media/Textures/Explosion.png");

}

void World::buildScene() 
{
	for (size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = i == LowerAir ?
			Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	sf::Texture& jungleTexture = mTextures.get(Textures::ID::Jungle);
	jungleTexture.setRepeated(true);

	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(jungleTexture, 
		static_cast<sf::IntRect>(mWorldBounds)));
	jungleSprite->setPosition(mWorldBounds.position);
	mSceneLayers[Background]
		->attachChild(std::move(jungleSprite));

	std::unique_ptr<ParticleNode>smokeNode(
		new ParticleNode(Particle::Type::Smoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	std::unique_ptr<ParticleNode>propellantNode(
		new ParticleNode(Particle::Type::Propellant, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));

	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mFonts, mTextures));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mSceneLayers[UpperAir]->attachChild(std::move(player));

	addEnemies();
}

void World::adaptPlayerVelocity() 
{
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		mPlayerAircraft->setVelocity(velocity / sqrtf(2.f));
	}

	mPlayerAircraft->accelerate({ 0.f, mScrollSpeed });
}

void World::adaptPlayerPosition()
{
	sf::FloatRect viewBounds({ 
		mWorldView.getCenter() - mWorldView.getSize() / 2.f,
		mWorldView.getSize() 	
	});
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, 
		viewBounds.position.x + borderDistance);
	position.x = std::min(position.x, 
		viewBounds.position.x + viewBounds.size.x - borderDistance);
	position.y = std::max(position.y, 
		viewBounds.position.y + borderDistance);
	position.y = std::min(position.y, 
		viewBounds.position.y + viewBounds.size.y - borderDistance);
	
	mPlayerAircraft->setPosition(position);
}

// Return whether the pair matches the categories and reorder the colliders
bool matchesCategories(SceneNode::Pair& colliders,
	Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// There are sub-categories to categories
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for (SceneNode::Pair pair : collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
		{
			auto& player = static_cast<Aircraft&> (*pair.first);
			auto& enemy = static_cast<Aircraft&> (*pair.second);

			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}
		else if (matchesCategories(pair, Category::PlayerAircraft, Category::Pickup))
		{
			auto& player = static_cast<Aircraft&> (*pair.first);
			auto& pickup = static_cast<Pickup&> (*pair.second);

			pickup.apply(player);
			pickup.destroy();
		}
		else if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile)
			|| matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile))
		{
			auto& aircraft = static_cast<Aircraft&> (*pair.first);
			auto& projectile = static_cast<Projectile&> (*pair.second);

			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::addEnemies()
{
	addEnemy(Aircraft::Raptor, 0.f, 500.f);
	addEnemy(Aircraft::Raptor, 0.f, 1000.f);
	addEnemy(Aircraft::Raptor, +100.f, 1100.f);
	addEnemy(Aircraft::Raptor, -100.f, 1100.f);
	addEnemy(Aircraft::Avenger, -70.f, 1400.f);
	addEnemy(Aircraft::Avenger, -70.f, 1600.f);
	addEnemy(Aircraft::Avenger, 70.f, 1400.f);
	addEnemy(Aircraft::Avenger, 70.f, 1600.f);

	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(),
		[](const SpawnPoint& lhs, const SpawnPoint& rhs)
		{
			return lhs.y < rhs.y;
		});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
	mEnemySpawnPoints.emplace_back(
		type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
}

void World::spawnEnemies()
{
	while (!mEnemySpawnPoints.empty() && 
		getBattlefieldBounds().position.y < mEnemySpawnPoints.back().y)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();

		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, mFonts, mTextures));
		enemy->setPosition({ spawn.x, spawn.y });
		enemy->setRotation(sf::degrees(180.f));

		mSceneLayers[UpperAir]->attachChild(std::move(enemy));

		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>(
		[this](Entity& e, sf::Time)
		{
			if (getBattlefieldBounds().findIntersection(e.getBoundingRect())
				== std::nullopt)
			{
				e.remove();
			}
		});

	mCommandQueue.push(command);
}

void World::guideMissiles()
{
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>(
		[this](Aircraft& enemy, sf::Time)
		{
			if (!enemy.isDestroyed())
			{
				mActiveEnemies.push_back(&enemy);
			}
		}
	);

	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>(
		[this](Projectile& missile, sf::Time)
		{
			if (!missile.isGuided())
			{
				return;
			}
			
			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for (Aircraft* enemy : mActiveEnemies)
			{
				float enemyDistance = distance(missile, *enemy);
				if (enemyDistance < minDistance)
				{
					closestEnemy = enemy;
					minDistance = enemyDistance;
				}
			}

			if (closestEnemy)
			{
				missile.guideTowards(closestEnemy->getWorldPosition());
			}
		}
	);

	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	
	mActiveEnemies.clear();	
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, 
		mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	sf::FloatRect bounds = getViewBounds();
	bounds.position.y -= 100.f;
	bounds.size.y += 100.f;

	return bounds;
}



