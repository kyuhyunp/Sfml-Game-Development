#include "../Header/MusicPlayer.h"

#include <iostream>

MusicPlayer::MusicPlayer()
	: mMusic()
	, mFilenames()
{
	mMusic.setRelativeToListener(true);
	mMusic.setAttenuation(0.f);

	mFilenames[Music::ID::MenuTheme] = "Media/Music/MenuTheme.ogg";
	mFilenames[Music::ID::MissionTheme] = "Media/Music/MissionTheme.ogg";
}

void MusicPlayer::play(Music::ID theme)
{
	std::string filename = mFilenames[theme];
	if (!mMusic.openFromFile(filename))
	{
		throw std::runtime_error("Music " + filename + " could not be loaded.");
	}

	mMusic.setLooping(true);
	mMusic.play();
}

void MusicPlayer::stop()
{
	mMusic.stop();
}

void MusicPlayer::setPaused(bool paused)
{
	if (paused)
	{
		mMusic.pause();
	}
	else
	{
		mMusic.play();
	}
}
void MusicPlayer::setVolume(float volume)
{
	mMusic.setVolume(std::clamp(volume, 0.f, 100.f));
}

float MusicPlayer::getVolume() const
{
	return mMusic.getVolume();
}

void MusicPlayer::printStatus() const
{
	switch (mMusic.getStatus())
	{
	case sf::Music::Status::Paused:
		std::cout << "Paused\n";
		break;
	case sf::Music::Status::Playing :
		std::cout << "Playing\n";
		break;
	case sf::Music::Status::Stopped:
		std::cout << "Stopped\n";
		break;
	}
}