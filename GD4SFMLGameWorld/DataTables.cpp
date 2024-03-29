#include "DataTables.hpp"
#include "Aircraft.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "AircraftID.hpp"
#include "ProjectileID.hpp"
#include "PickupID.hpp"
#include "ParticleID.hpp"

//Eoghan - D00187992

std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(static_cast<int>(AircraftID::TypeCount));

	data[static_cast<int>(AircraftID::Player)].hitpoints = 100;
	data[static_cast<int>(AircraftID::Player)].speed = 200.f;
	data[static_cast<int>(AircraftID::Player)].fireInterval = sf::seconds(3);
	data[static_cast<int>(AircraftID::Player)].textureRect = sf::IntRect(0, 0, 180, 100);
	data[static_cast<int>(AircraftID::Player)].texture = TextureID::Player;
	data[static_cast<int>(AircraftID::Player)].hasRollAnimation = false;

	data[static_cast<int>(AircraftID::Player2)].hitpoints = 100;
	data[static_cast<int>(AircraftID::Player2)].speed = 200.f;
	data[static_cast<int>(AircraftID::Player2)].fireInterval = sf::seconds(3);
	data[static_cast<int>(AircraftID::Player2)].textureRect = sf::IntRect(0, 0, 180, 100);
	data[static_cast<int>(AircraftID::Player2)].texture = TextureID::Player2;
	data[static_cast<int>(AircraftID::Player2)].hasRollAnimation = false;

	data[static_cast<int>(AircraftID::Enemy)].hitpoints = 200;
	data[static_cast<int>(AircraftID::Enemy)].speed = 80.f;
	data[static_cast<int>(AircraftID::Enemy)].fireInterval = sf::seconds(2);
	data[static_cast<int>(AircraftID::Enemy)].texture = TextureID::Enemy;
	data[static_cast<int>(AircraftID::Enemy)].textureRect = sf::IntRect(0, 0, 230, 366);

	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+179.f, 100.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+359.f, 100.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+179.f, 100.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+359.f, 100.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+90.f, 50.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(-90.f, 200.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+359.f, 50.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(+179.f, 50.f));
	data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(-90.f, 800.f));
	//data[static_cast<int>(AircraftID::Enemy)].directions.push_back(Direction(-160.f, 50.f));
	//data[static_cast<int>(AircraftID::Raptor)].hasRollAnimation = false;

	//data[static_cast<int>(AircraftID::Avenger)].hitpoints = 40;
	//data[static_cast<int>(AircraftID::Avenger)].speed = 50.f;
	//data[static_cast<int>(AircraftID::Avenger)].fireInterval = sf::seconds(2);
	//data[static_cast<int>(AircraftID::Avenger)].texture = TextureID::Entities;
	//data[static_cast<int>(AircraftID::Avenger)].textureRect = sf::IntRect(228, 0, 60, 59);
	//data[static_cast<int>(AircraftID::Avenger)].directions.push_back(Direction(+45.f, 50.f));
	//data[static_cast<int>(AircraftID::Avenger)].directions.push_back(Direction(0.f, 50.f));
	//data[static_cast<int>(AircraftID::Avenger)].directions.push_back(Direction(-45.f, 100.f));
	//data[static_cast<int>(AircraftID::Avenger)].directions.push_back(Direction(0.f, 50.f));
	//data[static_cast<int>(AircraftID::Avenger)].directions.push_back(Direction(+45.f, 50.f));
	//data[static_cast<int>(AircraftID::Avenger)].hasRollAnimation = false;

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileID::TypeCount));

	data[static_cast<int>(ProjectileID::AlliedBullet)].damage = 10;
	data[static_cast<int>(ProjectileID::AlliedBullet)].speed = 300.f;
	data[static_cast<int>(ProjectileID::AlliedBullet)].texture = TextureID::Entities;
	data[static_cast<int>(ProjectileID::AlliedBullet)].textureRect = sf::IntRect(175, 64, 3, 14);

	data[static_cast<int>(ProjectileID::EnemyBullet)].damage = 10;
	data[static_cast<int>(ProjectileID::EnemyBullet)].speed = -300.f;
	data[static_cast<int>(ProjectileID::EnemyBullet)].texture = TextureID::Entities;
	data[static_cast<int>(ProjectileID::EnemyBullet)].textureRect = sf::IntRect(175, 64, 3, 14);


	data[static_cast<int>(ProjectileID::Missile)].damage = 20;
	data[static_cast<int>(ProjectileID::Missile)].speed = 250.f;
	data[static_cast<int>(ProjectileID::Missile)].texture = TextureID::Entities;
	data[static_cast<int>(ProjectileID::Missile)].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupID::TypeCount));
	data[static_cast<int>(PickupID::HealthRefill)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::HealthRefill)].textureRect = sf::IntRect(0, 64, 40, 40);
	data[static_cast<int>(PickupID::HealthRefill)].action = [](Aircraft& a) {a.repair(25); };

	data[static_cast<int>(PickupID::MissileRefill)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::MissileRefill)].textureRect = sf::IntRect(40, 64, 40, 40);
	data[static_cast<int>(PickupID::MissileRefill)].action = std::bind(&Aircraft::collectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupID::FireSpread)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::FireSpread)].textureRect = sf::IntRect(80, 64, 40, 40);
	data[static_cast<int>(PickupID::FireSpread)].action = std::bind(&Aircraft::increaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupID::FireRate)].texture = TextureID::Entities;
	data[static_cast<int>(PickupID::FireRate)].textureRect = sf::IntRect(120, 64, 40, 40);
	data[static_cast<int>(PickupID::FireRate)].action = std::bind(&Aircraft::increaseFireRate, std::placeholders::_1);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleID::ParticleCount));

	data[static_cast<int>(ParticleID::Propellant)].color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleID::Propellant)].lifetime = sf::seconds(0.6f);

	data[static_cast<int>(ParticleID::Smoke)].color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleID::Smoke)].lifetime = sf::seconds(4.f);

	return data;
}
