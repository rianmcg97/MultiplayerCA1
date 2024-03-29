#include "Aircraft.hpp"
#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include "SFML/Graphics/RenderStates.hpp"
#include "TextureID.hpp"
#include "AircraftID.hpp"
#include "ProjectileID.hpp"
#include "PickupID.hpp"

#include <cmath>

namespace
{
	const std::vector<AircraftData> Table = initializeAircraftData();
}

TextureID toTextureID(AircraftID type)
{
	switch (type)
	{
	case AircraftID::Player:
		return TextureID::Player;

	case AircraftID::Player2:
		return TextureID::Player2;

	/*case AircraftID::Enemy:
		return TextureID::Enemy;*/
	}
	return TextureID::Player;
}

Aircraft::Aircraft(AircraftID type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(type)].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
	, mExplosion(textures.get(TextureID::Explosion))
	, mFireCommand()
	, mMissileCommand()
	, mFireCountdown(sf::Time::Zero)
	, mIsFiring(false)
	, mIsLaunchingMissile(false)
	, mShowExplosion(true)
	, mPlayedExplosionSound(false)
	, mSpawnedPickup(false)
	, mIsMarkedForRemoval(false)
	, mFireRateLevel(1)
	, mSpreadLevel(1)
	, mMissileAmmo(2)
	, mDropPickupCommand()
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
	, mHealthDisplay(nullptr)
	, mMissileDisplay(nullptr)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centreOrigin(mSprite);
	centreOrigin(mExplosion);

	mFireCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createProjectile(node, ProjectileID::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mDropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == (static_cast<int>(CategoryID::PlayerAircraft)))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	if (getCategory() == (static_cast<int>(CategoryID::Player2Aircraft)))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}
	updateTexts();
}


void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		//checkPickupDrop(commands);
		mExplosion.update(dt);
		mIsMarkedForRemoval = true;
		//Play explosion sound
		if (!mPlayedExplosionSound)
		{
			SoundEffectID soundEffect = (randomInt(2) == 0) ? SoundEffectID::Explosion1 : SoundEffectID::Explosion2;
			playerLocalSound(commands, soundEffect);

			mPlayedExplosionSound = true;
		}
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);

	// Update texts
	updateTexts();
	//updateRollAnimation();
}


unsigned int Aircraft::getCategory() const
{
	if (isAlliedPlayer1())
		return static_cast<int>(CategoryID::PlayerAircraft);
	else if (isAlliedPlayer2())
		return static_cast<int>(CategoryID::Player2Aircraft);
	else
		return static_cast<int>(CategoryID::EnemyAircraft);
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

bool Aircraft::isAlliedPlayer1() const
{
	return mType == AircraftID::Player;
}

bool Aircraft::isAlliedPlayer2() const
{
	return mType == AircraftID::Player2;
}

bool Aircraft::isAlliedPlayer() const {
	return mType == AircraftID::Player || mType == AircraftID::Player2;
}

float Aircraft::getMaxSpeed() const
{
	return Table[static_cast<int>(mType)].speed;
}

void Aircraft::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Aircraft::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Aircraft::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Aircraft::playerLocalSound(CommandQueue& commands, SoundEffectID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = static_cast<int>(CategoryID::SoundEffect);
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time)
	{
		node.playSound(effect, worldPosition);
	});
	commands.push(command);
}

void Aircraft::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[static_cast<int>(mType)].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Aircraft::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		//mMissileAmmo;
	}

}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[static_cast<int>(mType)].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

//void Aircraft::checkPickupDrop(CommandQueue& commands)
//{
//	if (!isAllied1() || !isAllied2() && randomInt(3) == 0 && !mSpawnedPickup)
//		commands.push(mDropPickupCommand);
//	mSpawnedPickup = true;
//}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	//Checks for either if they're Player 1 or Player 2) - Eoghan
	if (!isAlliedPlayer())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		playerLocalSound(commands, isAlliedPlayer1() || isAlliedPlayer2() ? SoundEffectID::AlliedLasers : SoundEffectID::EnemyGunfire);
		
		mFireCountdown += Table[static_cast<int>(mType)].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		playerLocalSound(commands, SoundEffectID::LaunchMissile);
		mIsLaunchingMissile = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) 
{
	
	ProjectileID type = isAlliedPlayer1() || isAlliedPlayer2() ? ProjectileID::AlliedBullet : ProjectileID::EnemyBullet;
	switch (mSpreadLevel)
	{
	case 1:
		createProjectile(node, type, 0.0f, 0.01f, textures);
		break;

	case 2:
		createProjectile(node, type, -0.33f, 0.01f, textures);
		createProjectile(node, type, +0.33f, 0.01f, textures);
		break;

	case 3:
		createProjectile(node, type, -0.5f, 0.01f, textures);
		createProjectile(node, type, 0.0f, 0.01f, textures);
		createProjectile(node, type, +0.5f, 0.01f, textures);
		break;
	}
	//Eoghan
	//If you're the boss
	if (!isAlliedPlayer()) {
		//And you're under 180 hitpoints
		if(this->getHitpoints() < 180 && this->getHitpoints() > 100 && mSpreadLevel== 1){
			increaseSpread();
		}

		if (this->getHitpoints() <= 100 && mSpreadLevel == 2) {
			increaseSpread();
		}
	}
}

void Aircraft::createProjectile(SceneNode& node, ProjectileID type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, 0.01f);
	sf::Vector2f velocity(projectile->getMaxSpeed(), 0);

	float sign1 = +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign1);
	projectile->setVelocity(velocity * sign1);
	projectile->setRotation(90);
	node.attachChild(std::move(projectile));

}

void Aircraft::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupID>(randomInt(static_cast<int>(PickupID::TypeCount)));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts()
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	if (mMissileDisplay)
	{
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}

//void Aircraft::updateRollAnimation()
//{
//	if (Table[static_cast<int>(mType)].hasRollAnimation)
//	{
//		sf::IntRect textureRect = Table[static_cast<int>(mType)].textureRect;
//
//		// Roll left: Texture rect offset once
//		if (getVelocity().x < 0.f)
//			textureRect.left += textureRect.width;
//
//		// Roll right: Texture rect offset twice
//		else if (getVelocity().x > 0.f)
//			textureRect.left += 2 * textureRect.width;
//
//		mSprite.setTextureRect(textureRect);
//	}
//}
