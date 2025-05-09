#ifndef SCENENODE_H
#define SCENENODE_H

#include "Category.h"
#include "Command.h"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <memory>
#include <vector>
#include <cassert>

class SceneNode : public sf::Transformable, public sf::Drawable
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;

	SceneNode();

	// Non copyable
	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(SceneNode&) = delete;

	void attachChild(Ptr child);

	Ptr detachChild(const SceneNode& node);

	void update(sf::Time dt);

	sf::Vector2f getWorldPosition() const;

	sf::Transform getWorldTransform() const;

	void onCommand(const Command& command, sf::Time dt);

	virtual unsigned int getCategory() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;

	virtual void updateCurrent(sf::Time dt);

	void updateChildren(sf::Time dt);

	std::vector<Ptr> mChildren;
	SceneNode* mParent;
};

#endif

