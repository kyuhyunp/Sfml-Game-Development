#ifndef SOUND_NODE_H
#define SOUND_NODE_H

#include "../Header/SceneNode.h"
#include "../Header/ResourceHolder.hpp"
#include "../Header/ResourceIdentifiers.h"
#include "../Header/SoundPlayer.h"


class SoundNode : public SceneNode
{
public:
	explicit SoundNode(SoundPlayer& sounds);

	void playSound(SoundEffect::ID sound, sf::Vector2f position);

	virtual unsigned int getCategory() const;

private:
	SoundPlayer& mSounds;
};

#endif
