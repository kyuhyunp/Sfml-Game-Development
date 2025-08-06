#ifndef EMITTER_NODE_H
#define EMITTER_NODE_H

#include "SceneNode.h"
#include "Particle.h"

#include "SFML/System/Time.hpp"


class ParticleNode;

class EmitterNode : public SceneNode
{
public:
	explicit EmitterNode(Particle::Type type);

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void emitParticles(sf::Time dt);

	sf::Time mAccumulatedTime;
	Particle::Type mType;
	ParticleNode* mParticleSystem;

};

#endif
