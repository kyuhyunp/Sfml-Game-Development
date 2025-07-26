#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "SceneNode.h"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);

	void setVelocity(sf::Vector2f velocity);
	void setVelocity(float vx, float vy);
	void accelerate(sf::Vector2f velocity);
	void accelerate(float vx, float vy);
	sf::Vector2f getVelocity() const;

	int getHitpoints() const;
	void repair(int points);
	void damage(int points);
	void destroy();
	virtual bool isDestroyed() const;

protected:
	void setCategory(Category::Type category);
	virtual void updateCurrent(sf::Time dt, CommandQueue&);

private:
	sf::Vector2f mVelocity;
	int mHitpoints;
};

#endif

