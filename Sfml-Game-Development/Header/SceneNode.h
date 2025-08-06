#ifndef SCENENODE_H
#define SCENENODE_H

#include "Category.h"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <memory>
#include <vector>
#include <cassert>
#include <set>

struct Command;
class CommandQueue;

class SceneNode : public sf::Transformable
				, public sf::Drawable
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

	SceneNode(Category::Type category = Category::None);

	// Non copyable
	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(SceneNode&) = delete;

	void attachChild(Ptr child);

	Ptr detachChild(const SceneNode& node);

	void update(sf::Time dt, CommandQueue& commands);

	sf::Vector2f getWorldPosition() const;

	sf::Transform getWorldTransform() const;

	void onCommand(const Command& command, sf::Time dt);

	virtual unsigned int getCategory() const;

	virtual void checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	virtual void checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);
	void removeWrecks();
	
	virtual sf::FloatRect getBoundingRect() const;
	
	virtual bool isMarkedForRemoval() const;
	virtual bool isDestroyed() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const;

	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateChildren(sf::Time dt, CommandQueue& commands);

	std::vector<Ptr> mChildren;
	SceneNode* mParent;
	Category::Type mDefaultCategory;
};

// Global function used for SceneNodes
float distance(const SceneNode& lhs, const SceneNode& rhs);
bool collision(const SceneNode& lhs, const SceneNode& rhs);

#endif

