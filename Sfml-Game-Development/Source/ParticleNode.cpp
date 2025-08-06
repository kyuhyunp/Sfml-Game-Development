#include "../Header/ParticleNode.h"
#include "../Header/DataTables.h"
#include "../Header/ResourceHolder.hpp"

#include "SFML/Graphics/Texture.hpp"


namespace
{
	const std::vector<ParticleData> Table = initializeParticleData();
}
// four points using size, half of the texture

ParticleNode::ParticleNode(Particle::Type type, const TextureHolder& textures)
	: SceneNode()
	, mParticles()
	, mTexture(textures.get(Textures::ID::Particle))
	, mType(type)
	, mVertexArray(sf::PrimitiveType::Triangles)
	, mNeedsVertexUpdate(true)
{
}

void ParticleNode::addParticle(sf::Vector2f position)
{
	mParticles.emplace_back(position, Table[mType].color, Table[mType].lifetime);
}

Particle::Type ParticleNode::getParticleType() const
{
	return mType;
}

unsigned int ParticleNode::getCategory() const
{
	return Category::ParticleSystem;
}

void ParticleNode::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	while (!mParticles.empty() && mParticles.front().lifetime <= sf::Time::Zero)
	{
		mParticles.pop_front();
	}

	for (Particle& particle : mParticles)
	{
		particle.lifetime -= dt;
	}

	mNeedsVertexUpdate = true;
}

void ParticleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (mNeedsVertexUpdate)
	{
		computeVertices();
		mNeedsVertexUpdate = false;
	}

	states.texture = &mTexture;
	target.draw(mVertexArray, states);
}

void ParticleNode::addVertex(float worldX, float worldY,
	float texCoordX, float texCoordY, const sf::Color& color) const
{
	sf::Vertex vertex;
	vertex.position = sf::Vector2f(worldX, worldY);
	vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);
	vertex.color = color;

	mVertexArray.append(vertex);
}

void ParticleNode::computeVertices() const
{
	sf::Vector2f size (mTexture.getSize());
	sf::Vector2f half = size / 2.f;

	mVertexArray.clear();

	for (const Particle& particle : mParticles)
	{
		sf::Vector2f pos = particle.position;
		sf::Color c = particle.color;
		float ratio = particle.lifetime.asSeconds() / Table[mType].lifetime.asSeconds();
		c.a = static_cast<uint8_t> (255 * std::max(ratio, 0.f));

		addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, c);
		addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, c);
		addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, c);

		addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, c);
		addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, c);
		addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, c);
	}
}