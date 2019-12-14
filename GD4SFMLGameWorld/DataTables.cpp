#include "DataTables.hpp"
#include "Aircraft.hpp"
#include "Projectile.hpp"
//#include "Pickup.hpp"
#include "Particle.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(Aircraft::TypeCount);

	data[Aircraft::Player].hitpoints = 100;
	data[Aircraft::Player].speed = 200.f;
	data[Aircraft::Player].fireInterval = sf::seconds(1);
	//data[Aircraft::Player].texture = Textures::Player;
	data[Aircraft::Player].textureRect = sf::IntRect(0, 0, 48, 64);
	data[Aircraft::Player].hasRollAnimation = true;

	data[Aircraft::Player2].hitpoints = 100;
	data[Aircraft::Player2].speed = 200.f;
	data[Aircraft::Player2].fireInterval = sf::seconds(1);
	//data[Aircraft::Player2].texture = Textures::Player2;
	data[Aircraft::Player2].textureRect = sf::IntRect(0, 0, 48, 64);
	data[Aircraft::Player2].hasRollAnimation = true;

	data[Aircraft::Enemy].hitpoints = 200;
	data[Aircraft::Enemy].speed = 50.f;
	//data[Aircraft::Enemy].texture = Textures::Enemy;
	data[Aircraft::Enemy].textureRect = sf::IntRect(228, 0, 60, 59);
	data[Aircraft::Enemy].directions.push_back(Direction(+45.f, 50.f));
	data[Aircraft::Enemy].directions.push_back(Direction(0.f, 50.f));
	data[Aircraft::Enemy].directions.push_back(Direction(-45.f, 100.f));
	data[Aircraft::Enemy].directions.push_back(Direction(0.f, 50.f));
	data[Aircraft::Enemy].directions.push_back(Direction(+45.f, 50.f));
	data[Aircraft::Enemy].fireInterval = sf::seconds(2);
	data[Aircraft::Enemy].hasRollAnimation = false;

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
	data[Projectile::AlliedBullet].texture = Textures::Entities;
	data[Projectile::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::Entities;
	data[Projectile::EnemyBullet].textureRect = sf::IntRect(178, 64, 3, 14);

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::Entities;
	data[Projectile::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

//std::vector<PickupData> initializePickupData()
//{
//	std::vector<PickupData> data(Pickup::TypeCount);
//
//	data[Pickup::HealthRefill].texture = Textures::Entities;
//	data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
//	data[Pickup::HealthRefill].action = [](Aircraft& a) { a.repair(25); };
//
//	data[Pickup::MissileRefill].texture = Textures::Entities;
//	data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
//	data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);
//
//	data[Pickup::FireSpread].texture = Textures::Entities;
//	data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
//	data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);
//
//	data[Pickup::FireRate].texture = Textures::Entities;
//	data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
//	data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);
//
//	return data;
//}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}
