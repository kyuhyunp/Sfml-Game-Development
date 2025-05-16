#include "../Header/GameState.h"

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(*context.window)
	, mPlayer(*context.player)
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

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPush(States::Pause);
		}
	}

	return true;
}