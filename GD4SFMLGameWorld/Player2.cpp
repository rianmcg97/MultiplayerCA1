#include "Player2.hpp"
#include "Aircraft.hpp"
#include "CommandQueue.hpp"

struct AircraftMover
{
	AircraftMover(float vx, float vy) : velocity(vx, vy) {}

	void operator()(Aircraft& aircraft, sf::Time) const
	{
		aircraft.accelerate(velocity);
	}
	sf::Vector2f velocity;
};

Player2::Player2()
{
	//Set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = ActionID::MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = ActionID::MoveRight;
	mKeyBinding[sf::Keyboard::Up] = ActionID::MoveUp;
	mKeyBinding[sf::Keyboard::Down] = ActionID::MoveDown;

	//Set initial action bindings
	initializeActions();

	//Assign all categories to the player's aircraft
	for (auto& pair : mActionBindings)
	{
		pair.second.category = static_cast<int>(CategoryID::AlliedAircraft);
	}
}


void Player2::initializeActions()
{
	const float player2Speed = 200.f;
	mActionBindings[ActionID::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-player2Speed, 0.f));
	mActionBindings[ActionID::MoveRight].action = derivedAction<Aircraft>(AircraftMover(player2Speed, 0.f));
	mActionBindings[ActionID::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, -player2Speed));
	mActionBindings[ActionID::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, player2Speed));
}

void Player2::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		//Check if key pressed is in the key bindings, trigger event if it is
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBindings[found->second]);
		}
	}
}

void Player2::handleRealtimeInput(CommandQueue& commands)
{
	//Check all assigned keys and see if they are pressed
	for (auto pair : mKeyBinding)
	{
		//if the key is pressed, look up the action and trigger the corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(mActionBindings[pair.second]);
		}
	}
}

void Player2::assignKey(ActionID action, sf::Keyboard::Key key)
{
	//Remove all keys that already map to an action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end();)
	{
		if (itr->second == action)
		{
			mKeyBinding.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player2::getAssignedKey(ActionID action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
}

bool Player2::isRealtimeAction(ActionID action)
{
	switch (action)
	{
	case ActionID::MoveLeft:
	case ActionID::MoveRight:
	case ActionID::MoveUp:
	case ActionID::MoveDown:
		return true;

	default:
		return false;
	}
}

