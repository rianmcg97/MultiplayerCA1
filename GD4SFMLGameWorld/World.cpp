#include "World.hpp"

World::World(sf::RenderWindow& window) : mWindow(window), mCamera(window.getDefaultView()), 
mTextures(), mSceneGraph(), mSceneLayers(), mWorldBounds(0.f, 0.f, 2000.f, mCamera.getSize().x),
mSpawnPosition(mCamera.getSize().x/2.f, mWorldBounds.height - mCamera.getSize().y/2.f), mScrollSpeed(-50.f),
mPlayerAircraft(nullptr), mPlayer2Aircraft(nullptr)
{
	loadTextures();
	buildScene();

	//Prepare the Camera
	mCamera.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	//Scroll the world
	mCamera.move(-mScrollSpeed * dt.asSeconds(), 0.f);

	mPlayerAircraft->setVelocity(-mScrollSpeed * dt.asSeconds(), 0.f);
	mPlayer2Aircraft->setVelocity(-mScrollSpeed * dt.asSeconds(), 0.f);

	mEnemyShip->setVelocity(0.f, 0.f);

	//Forward commands to the scene graph, adapt velocity
	while (!mCommandQueue.isEmpty())
	{
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	}
	adaptPlayerVelocity();
	adaptPlayer2Velocity();
	//Regular update, adapt position if outisde view	
	mSceneGraph.update(dt);
	adaptPlayerPosition();
	adaptPlayer2Position();
	
}

void World::draw()
{
	mWindow.setView(mCamera);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::loadTextures()
{
	mTextures.load(TextureID::Raptor, "Media/Textures/Raptor.png");
	mTextures.load(TextureID::TitleScreen, "Media/Textures/TitleScreen.png");
	mTextures.load(TextureID::Enemy, "Media/Textures/Enemy.png");
	mTextures.load(TextureID::Player, "Media/Textures/Player.png");
	mTextures.load(TextureID::Player2, "Media/Textures/Player2.png");
}

void World::buildScene()
{
	//Initialize the different
	for (std::size_t i = 0; i < static_cast<int>(LayerID::LayerCount); ++i)
	{
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();
		mSceneGraph.attachChild(std::move(layer));
	}

	//Prepare the tiled background
	sf::Texture& texture = mTextures.get(TextureID::TitleScreen);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);
	//Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[static_cast<int>(LayerID::Background)]->attachChild(std::move(backgroundSprite));

	//Add players aircraft
	std::unique_ptr<Aircraft> player(new Aircraft(AircraftID::Player, mTextures));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mPlayerAircraft->setRotation(90);
	mPlayerAircraft->setVelocity(40.f, mScrollSpeed);
	mPlayerAircraft->setScale(0.5, 1);
	mSceneLayers[static_cast<int>(LayerID::Air)]->attachChild(std::move(player));

	//Player 2
	std::unique_ptr<Aircraft> player2(new Aircraft(AircraftID::Player2, mTextures));
	mPlayer2Aircraft = player2.get();
	mPlayer2Aircraft->setPosition(mSpawnPosition);
	mPlayer2Aircraft->setRotation(90);
	mPlayer2Aircraft->setVelocity(40.f, mScrollSpeed);
	mPlayer2Aircraft->setScale(0.5, 1);
	mSceneLayers[static_cast<int>(LayerID::Air)]->attachChild(std::move(player2));

	//Add enemy
	std::unique_ptr<Aircraft> Enemy(new Aircraft(AircraftID::Enemy, mTextures));
	mEnemyShip = Enemy.get();
	mEnemyShip->setPosition(600.f, 300.f);
	mEnemyShip->setRotation(270);
	mEnemyShip->setVelocity(40.f, mScrollSpeed);
	mSceneLayers[static_cast<int>(LayerID::Air)]->attachChild(std::move(Enemy));
/*
	std::unique_ptr<Aircraft> rightEscort(new Aircraft(AircraftID::Raptor, mTextures));
	rightEscort->setPosition(80.f, 50.f);
	mPlayerAircraft->attachChild(std::move(rightEscort));*/
	
}

void World::adaptPlayerPosition()
{
	//Keep the player's position inside the screen bounds
	sf::FloatRect viewBounds(mCamera.getCenter() - mCamera.getSize() / 2.f, mCamera.getSize());
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
	//Keep the player's position inside the screen bounds
	sf::FloatRect viewBounds(mCamera.getCenter() - mCamera.getSize() / 2.f, mCamera.getSize());
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
	return sf::FloatRect(mCamera.getCenter() - mCamera.getSize() / 2.f, mCamera.getSize());
}