#include "../Header/SceneNode.h"
#include "../Header/Utility.hpp"


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

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return (lhs.getWorldPosition() - rhs.getWorldPosition()).length();
}


