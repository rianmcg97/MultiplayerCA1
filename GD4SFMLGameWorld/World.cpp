#include "World.hpp"
#include "Projectile.hpp"
//#include "Pickup.hpp"
#include "Foreach.hpp"
#include "TextNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts /*SoundPlayer& sounds*/)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	//, mSounds(sounds)
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 5000.f)
	, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f)
	, mScrollSpeed(-50.f)
	, mPlayerAircraft(nullptr)
	, mPlayer2Aircraft(nullptr)
	, mEnemySpawnPoints()
	, mActiveEnemies()
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	mWorldView.move(-mScrollSpeed * dt.asSeconds(), 0.f);
	mPlayerAircraft->setVelocity(0.f, 0.f);
	mPlayer2Aircraft->setVelocity(0.f, 0.f);

	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();
	adaptPlayer2Velocity();

	// Collision detection and response (may destroy entities)
	handleCollisions();

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	spawnEnemies();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
	adaptPlayer2Position();
	
}

void World::draw()
{
	//if (PostEffect::isSupported())
	//{
		mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		//mBloomEffect.apply(mSceneTexture, mTarget);
	//}
	//else
	//{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
	//}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasPlayerReachedEnd() const
{
	return !mWorldBounds.contains(mPlayerAircraft->getPosition());
}

bool World::hasPlayer2ReachedEnd() const
{
	return !mWorldBounds.contains(mPlayer2Aircraft->getPosition());
}

void World::loadTextures()
{
	mTextures.load(Textures::TitleScreen, "Media/Textures/TitleScreen.png");
	mTextures.load(Textures::Enemy, "Media/Textures/Enemy.png");
	mTextures.load(Textures::Player, "Media/Textures/Player.png");
	mTextures.load(Textures::Player2, "Media/Textures/Player2.png");
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& spaceTexture = mTextures.get(Textures::TitleScreen);
	spaceTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(spaceTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(jungleSprite));

	//Add players aircraft
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Player, mTextures, mFonts));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mPlayerAircraft->setRotation(90);
	mSceneLayers[UpperAir]->attachChild(std::move(player));

	//Player 2
	std::unique_ptr<Aircraft> player2(new Aircraft(Aircraft::Player2, mTextures, mFonts));
	mPlayer2Aircraft = player2.get();
	mPlayer2Aircraft->setPosition(mSpawnPosition);
	mPlayer2Aircraft->setRotation(90);
	mSceneLayers[UpperAir]->attachChild(std::move(player2));

	////Add enemy
	//std::unique_ptr<Aircraft> Enemy(new Aircraft(Aircraft::Enemy, mTextures));
	//mEnemyShip = Enemy.get();
	//mEnemyShip->setPosition(600.f, 300.f);
	//mEnemyShip->setRotation(270);
	//mEnemyShip->setVelocity(40.f, mScrollSpeed);
	//mSceneLayers[static_cast<int>(LayerID::Air)]->attachChild(std::move(Enemy));
/*
	std::unique_ptr<Aircraft> rightEscort(new Aircraft(AircraftID::Raptor, mTextures));
	rightEscort->setPosition(80.f, 50.f);
	mPlayerAircraft->attachChild(std::move(rightEscort));*/
	addEnemy(Aircraft::Enemy, 0.f, 500.f);
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	//Don't give the player an advantage of they move diagonally
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();

	//If moving diagonally, reduce the velocity by root 2
	if (velocity.x != 0 && velocity.y != 0)
	{
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));
	}
	//add the scrolling velocity
	mPlayerAircraft->accelerate(-mScrollSpeed, 0.f);
}

void World::adaptPlayer2Position()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayer2Aircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayer2Aircraft->setPosition(position);
}

void World::adaptPlayer2Velocity()
{
	//Don't give the player an advantage of they move diagonally
	sf::Vector2f velocity = mPlayer2Aircraft->getVelocity();

	//If moving diagonally, reduce the velocity by root 2
	if (velocity.x != 0 && velocity.y != 0)
	{
		mPlayer2Aircraft->setVelocity(velocity / std::sqrt(2.f));
	}
	//add the scrolling velocity
	mPlayer2Aircraft->accelerate(-mScrollSpeed, 0.f);
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}