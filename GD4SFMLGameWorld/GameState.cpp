#include "GameState.hpp"

GameState::GameState(StateStack& stack, Context context)
	:State(stack, context)
	, mWorld(*context.window)
	, mPlayer(*context.player)
	, mPlayer2(*context.player2)
{
}

void GameState::draw()
{
	mWorld.draw();
}

bool GameState::update(sf::Time dt)
{
	mWorld.update(dt);

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);
	mPlayer2.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);
	mPlayer2.handleEvent(event, commands);
	//Pause if esc is pressed
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		requestStackPush(StateID::Pause);
	}
	return true;
}
