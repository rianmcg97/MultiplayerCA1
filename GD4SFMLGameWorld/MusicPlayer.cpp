//Eoghan - D00187992

#include "MusicPlayer.hpp"



MusicPlayer::MusicPlayer()
	: mMusic()
	, mFilenames()
	, mVolume(100.f)
{
	mFilenames[MusicID::MenuTheme] = "Media/Music/MenuMusic.ogg";
	mFilenames[MusicID::MissionTheme] = "Media/Music/MissionMusic.ogg";
}

void MusicPlayer::play(MusicID theme)
{
	std::string filename = mFilenames[theme];

	if (!mMusic.openFromFile(filename))
	{
		throw std::runtime_error("Music " + filename + " could not be opened");
	}

	mMusic.setVolume(mVolume);
	mMusic.setLoop(true);
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
	mVolume = volume;
}
