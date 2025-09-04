#include "../Header/MusicPlayer.h"


MusicPlayer::MusicPlayer()
	: mMusic()
	, mFilenames()
{
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
