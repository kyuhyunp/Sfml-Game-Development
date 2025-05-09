#include "../Header/World.h"

World::World(sf::RenderWindow& window)
	: mWindow(window)
	, mWorldView(window.getDefaultView())
	, mTextures()
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds({ 0.f, 0.f }, { mWorldView.getSize().x, 2000.f })
	, mSpawnPosition(mWorldView.getSize().x / 2.f, 
		mWorldBounds.size.y - mWorldView.getSize().y / 2.f)
	, mScrollSpeed(-50.f)
	, mPlayerAircraft(nullptr)
{
	loadTextures();
	buildScene();

	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	mWorldView.move({ 0.f, mScrollSpeed * dt.asSeconds()});
	mPlayerAircraft->setVelocity({ 0.f, 0.f });

	while (!mCommandQueue.isEmpty())
	{
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	}

	adaptPlayerVelocity();
	mSceneGraph.update(dt);
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

void World::loadTextures() 
{
	mTextures.load(Textures::ID::Eagle, "Media/Textures/Eagle.png");
	mTextures.load(Textures::ID::Raptor, "Media/Textures/Raptor.png");
	mTextures.load(Textures::ID::Desert, "Media/Textures/Desert.png");
}

void World::buildScene() 
{
	for (size_t i = 0; i < LayerCount; ++i)
	{
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	sf::Texture& texture = mTextures.get(Textures::ID::Desert);
	texture.setRepeated(true);

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, 
		static_cast<sf::IntRect>(mWorldBounds)));
	backgroundSprite->setPosition(mWorldBounds.position);
	mSceneLayers[Background]
		->attachChild(std::move(backgroundSprite));

	std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Type::Eagle, mTextures));
	mPlayerAircraft = leader.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mPlayerAircraft->setVelocity({ 40.f, mScrollSpeed });
	mSceneLayers[Air]->attachChild(std::move(leader));

	std::unique_ptr<Aircraft> leftEscort(new Aircraft(Aircraft::Type::Raptor, mTextures));
	leftEscort->setPosition({ -80.f, 50.f });
	mPlayerAircraft->attachChild(std::move(leftEscort));

	std::unique_ptr<Aircraft> rightEscort(new Aircraft(Aircraft::Type::Raptor, mTextures));
	rightEscort->setPosition({ 80.f, 50.f });
	mPlayerAircraft->attachChild(std::move(rightEscort));
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