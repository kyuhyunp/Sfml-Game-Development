#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.h"

#include "SFML/Audio/Sound.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include "SFML/System/Vector2.hpp"

#include <list>


class SoundPlayer
{
public:
	// NonCopyable
	SoundPlayer(const SoundPlayer&) = delete;
	SoundPlayer& operator=(const SoundPlayer&) = delete;

	SoundPlayer();

	void play(SoundEffect::ID effect);
	void play(SoundEffect::ID effect, sf::Vector2f position);

	void removeStoppedSounds();
	void setListenerPosition(sf::Vector2f position);
	sf::Vector2f getListenerPosition() const;

private:
	SoundBufferHolder mSoundBuffers;
	std::list<sf::Sound> mSounds;
};

#endif
