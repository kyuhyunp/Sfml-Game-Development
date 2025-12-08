#include "../Header/SoundPlayer.h"

#include "SFML/Audio/Listener.hpp"
#include "SFML/System/Vector3.hpp"

#include <algorithm>
#include <iostream>


namespace
{ // Can optimize later
	float ListenerZ = 300.f;
	// Higher Attenuation = quicker drop in volume with distance
	float Attenuation = 8.f;
	float MinDistance2D = 200.f;
	float MinDistance3D = sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
	: mSoundBuffers()
	, mSounds()
{
	mSoundBuffers.load(SoundEffect::ID::AlliedGunfire,
		"Media/Sound/AlliedGunfire.wav");
	mSoundBuffers.load(SoundEffect::ID::EnemyGunfire,
		"Media/Sound/EnemyGunfire.wav");
	mSoundBuffers.load(SoundEffect::ID::Explosion1,
		"Media/Sound/Explosion1.wav");
	mSoundBuffers.load(SoundEffect::ID::Explosion2,
		"Media/Sound/Explosion2.wav");
	mSoundBuffers.load(SoundEffect::ID::LaunchMissile,
		"Media/Sound/LaunchMissile.wav");
	mSoundBuffers.load(SoundEffect::ID::CollectPickup,
		"Media/Sound/CollectPickup.wav");
	mSoundBuffers.load(SoundEffect::ID::Button,
		"Media/Sound/Button.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection({ 0.f, 0.f, -1.f });
}

void SoundPlayer::play(SoundEffect::ID effect)
{
	mSounds.push_back(sf::Sound(mSoundBuffers.get(effect)));
	sf::Sound& sound = mSounds.back();

	sound.setRelativeToListener(true); // ignore world coords
	sound.setAttenuation(0.f);         // no distance fade

	sound.play();
}

void SoundPlayer::play(SoundEffect::ID effect, sf::Vector2f position)
{
	mSounds.push_back(sf::Sound(mSoundBuffers.get(effect)));
	sf::Sound& sound = mSounds.back();

	sf::Vector3f listenerPosition = sf::Listener::getPosition();

	sound.setPosition({ position.x, -position.y, 0.f });
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::removeStoppedSounds()
{
	remove_if(mSounds.begin(), mSounds.end(), [](const sf::Sound& sound)
		{
			return sound.getStatus() == sf::Sound::Status::Stopped;
		}
	);
}

void SoundPlayer::setListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition({ position.x, -position.y, ListenerZ });
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}