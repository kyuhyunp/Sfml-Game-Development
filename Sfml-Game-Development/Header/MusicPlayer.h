#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "ResourceIdentifiers.h"

#include "SFML/Audio/Music.hpp"

#include <map>
#include <string>


class MusicPlayer
{
public:
	// NonCopyable
	MusicPlayer(const MusicPlayer&) = delete;
	MusicPlayer& operator=(const MusicPlayer&) = delete;

	MusicPlayer();

	void play(Music::ID theme);
	void stop();

	void setPaused(bool paused);
	void setVolume(float volume);

	float getVolume() const;

	void printStatus() const;

private:
	sf::Music mMusic;
	std::map<Music::ID, std::string> mFilenames;
};

#endif