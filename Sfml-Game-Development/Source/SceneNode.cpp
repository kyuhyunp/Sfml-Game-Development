#include "../Header/SceneNode.h"
#include "../Header/Command.h"
#include "../Header/Utility.hpp"

#include "SFML/Graphics/RectangleShape.hpp"


SceneNode::SceneNode(Category::Type category) 
	: mChildren()
	, mParent(nullptr) 
	, mDefaultCategory(category)
{
}

void SceneNode::attachChild(Ptr child) 
{
	child->mParent = this;
	mChildren.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node) 
{
	auto found = std::find_if(mChildren.begin(), mChildren.end(), 
		[&](Ptr& p) -> bool { return p.get() == &node; });
	assert(found != mChildren.end());

	Ptr result = std::move(*found);
	result->mParent = nullptr;
	mChildren.erase(found);
	return result;
}

void SceneNode::update(sf::Time dt, CommandQueue& commands)
{
	updateCurrent(dt, commands);
	updateChildren(dt, commands);
}

sf::Transform SceneNode::getWorldTransform() const 
{
	sf::Transform transform = sf::Transform::Identity;
	for (const SceneNode* node = this; node != nullptr; node = node->mParent) 
	{ // For Matrix Mulitplication, AB != BA and need to multiply starting from root
		transform = node->getTransform() * transform;
	}

	return transform;
}

sf::Vector2f SceneNode::getWorldPosition() const 
{
	return getWorldTransform() * sf::Vector2f();
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
	states.transform *= getTransform();

	drawCurrent(target, states);
	drawChildren(target, states);

	//drawBoundingRect(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{
}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const Ptr& child : mChildren)
	{
		child->draw(target, states);
	}
}
void SceneNode::drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::FloatRect boundary = getBoundingRect();

	sf::RectangleShape rectangle(boundary.size);
	rectangle.setPosition(boundary.position);
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineColor(sf::Color::Green);
	rectangle.setOutlineThickness(1.f);

	target.draw(rectangle);
}

void SceneNode::updateCurrent(sf::Time, CommandQueue&)
{
}

void SceneNode::updateChildren(sf::Time dt, CommandQueue& commands)
{
	for (Ptr& child : mChildren) {
		child->update(dt, commands);
	}
}

void SceneNode::onCommand(const Command& command, sf::Time dt) 
{
	unsigned int category = getCategory();
	if (command.category & category) 
	{
		command.action(*this, dt);
	}

	for (Ptr& child : mChildren)
	{
		child->onCommand(command, dt);
	}
}

unsigned int SceneNode::getCategory() const
{
	return mDefaultCategory;
}

/*
* Call checkNodeCollision for SceneGraph and its children
*/
void SceneNode::checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs)
{
	checkNodeCollision(sceneGraph, collisionPairs);

	for (Ptr& child : sceneGraph.mChildren)
	{
		checkSceneCollision(*child, collisionPairs);
	}
}

/*
* Compare node with this SceneNode and all its children
*/
void SceneNode::checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs)
{
	if (this != &node &&
		!isDestroyed() && !node.isDestroyed() 
		&& collision(*this, node))
	{
		collisionPairs.insert(std::minmax(this, &node));
	}

	for (Ptr& child : mChildren)
	{ // Have its children call checkNodeCollision and check with node
		child->checkNodeCollision(node, collisionPairs);
	}
}

void SceneNode::removeWrecks()
{
	// mem_fn used as it is applied to each one of the iterators 
	// from mChildren.begin() to mChildren.end()
	auto wreckfieldBegin = std::remove_if(mChildren.begin(), mChildren.end(),
		std::mem_fn(&SceneNode::isMarkedForRemoval));
	mChildren.erase(wreckfieldBegin, mChildren.end());
	std::for_each(mChildren.begin(), mChildren.end(),
		std::mem_fn(&SceneNode::removeWrecks));
}

sf::FloatRect SceneNode::getBoundingRect() const
{
	return sf::FloatRect();
}

bool SceneNode::isMarkedForRemoval() const
{
	return isDestroyed();
}

bool SceneNode::isDestroyed() const
{
	return false;
}

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return (lhs.getWorldPosition() - rhs.getWorldPosition()).length();
}


bool collision(const SceneNode& lhs, const SceneNode& rhs)
{
	const sf::FloatRect& lhsRect = lhs.getBoundingRect();
	const sf::FloatRect& rhsRect = rhs.getBoundingRect();
	
	return lhsRect.findIntersection(rhsRect) != std::nullopt;
}





