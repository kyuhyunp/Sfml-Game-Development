#include "../Header/SceneNode.h"

SceneNode::SceneNode() 
	: mChildren()
, mParent(nullptr) {}


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

void SceneNode::update(sf::Time dt) 
{
	updateCurrent(dt);
	updateChildren(dt);
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

sf::Vector2f SceneNode::getWorldPosition() const {
	return getWorldTransform() * sf::Vector2f();
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
	states.transform *= getTransform();

	drawCurrent(target, states);
	drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const Ptr& child : mChildren)
	{
		child->draw(target, states);
	}
}

void SceneNode::updateCurrent(sf::Time dt) 
{}


void SceneNode::updateChildren(sf::Time dt) 
{
	for (Ptr& child : mChildren) {
		child->update(dt);
	}
}


