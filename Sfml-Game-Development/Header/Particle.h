#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>


struct Particle
{
	enum Type
	{
		Propellant,
		Smoke,
		ParticleCount
	};

	sf::Vector2f position;
	sf::Color color;
	sf::Time lifetime;
};

#endif
