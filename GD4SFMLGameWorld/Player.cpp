#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Aircraft.hpp"
#include "ActionID.hpp"

#include <map>
#include <string>
#include <algorithm>
#include <iostream>


struct AircraftMover
{
	AircraftMover(float vx, float vy)
		: velocity(vx, vy)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		aircraft.accelerate(velocity * aircraft.getMaxSpeed());
	}

	sf::Vector2f velocity;
};

Player::Player() : mCurrentMissionStatus(MissionStatusID::MissionRunning)
{
	// Set initial key bindings
	mKeyBinding[sf::Keyboard::A] = ActionID::MoveLeft;
	mKeyBinding[sf::Keyboard::D] = ActionID::MoveRight;
	mKeyBinding[sf::Keyboard::W] = ActionID::MoveUp;
	mKeyBinding[sf::Keyboard::S] = ActionID::MoveDown;
	mKeyBinding[sf::Keyboard::Space] = ActionID::Fire;
	mKeyBinding[sf::Keyboard::C] = ActionID::LaunchMissile;

	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's aircraft
	for (auto& pair : mActionBinding)
		pair.second.category = static_cast<int>(CategoryID::PlayerAircraft);
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);

		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBinding[found->second]);
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	for (auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(mActionBinding[pair.second]);
		}
	}
}

void Player::assignKey(ActionID action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(ActionID action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void Player::setMissionStatus(MissionStatusID status)
{
	mCurrentMissionStatus = status;
}

MissionStatusID Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	mActionBinding[ActionID::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-1, 0));
	mActionBinding[ActionID::MoveRight].action = derivedAction<Aircraft>(AircraftMover(+1, 0));
	mActionBinding[ActionID::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0, -1));
	mActionBinding[ActionID::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0, +1));
	mActionBinding[ActionID::Fire].action = derivedAction<Aircraft>([](Aircraft& a, sf::Time) { a.fire(); });
	mActionBinding[ActionID::LaunchMissile].action = derivedAction<Aircraft>([](Aircraft& a, sf::Time) { a.launchMissile(); });
}

bool Player::isRealtimeAction(ActionID action)
{
	switch (action)
	{
	case ActionID::MoveLeft:
	case ActionID::MoveRight:
	case ActionID::MoveDown:
	case ActionID::MoveUp:
	case ActionID::Fire:
		return true;

	default:
		return false;
	}
}