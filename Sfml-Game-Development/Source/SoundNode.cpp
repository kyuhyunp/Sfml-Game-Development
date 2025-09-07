#include "../Header/SoundNode.h"

SoundNode::SoundNode(SoundPlayer& sounds)
	: mSounds(sounds)
{
}

void SoundNode::playSound(SoundEffect::ID sound, sf::Vector2f position)
{
	mSounds.play(sound, position);
}

unsigned int SoundNode::getCategory() const
{
	return Category::SoundEffect;
}