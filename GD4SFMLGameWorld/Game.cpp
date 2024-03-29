#include "Game.hpp"
#include "Utility.hpp"

const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	:mWindow(sf::VideoMode(640, 480), "SFML Hello World With Structure"), mWorld(mTarget, mFonts), mPlayer(), mPlayer2(), 
	mFont(), mStatisticsText(), mStatisticsUpdateTime(), mStatisticsNumFrames(0)
{
	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(20);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processInput();
			update(TimePerFrame);
		}
		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processInput()
{
	CommandQueue& commands = mWorld.getCommandQueue();
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		mPlayer.handleEvent(event, commands);
		mPlayer2.handleEvent(event, commands);

		if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}
	}
	mPlayer.handleRealtimeInput(commands);
	mPlayer2.handleRealtimeInput(commands);
}

void Game::update(sf::Time deltaTime)
{
	mWorld.update(deltaTime);
}

void Game::render()
{
	mWindow.clear();
	mWorld.draw();
	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);
	mWindow.display();
}


void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString("Frames/Second = " + toString(mStatisticsNumFrames) + "\n"+
			"Time/Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

			mStatisticsUpdateTime -= sf::seconds(1.0f);
			mStatisticsNumFrames = 0;
	}
}
