#include "../Header/NetworkNode.h"


NetworkNode::NetworkNode()
	: SceneNode()
	, mPendingActions()
{
}

void NetworkNode::notifyGameAction(GameActions::Type type, sf::Vector2f position)
{
	mPendingActions.push(GameActions::Action(type, position));
}

bool NetworkNode::pollGameAction(GameActions::Action& out)
{
	if (mPendingActions.empty())
	{
		return false;
	}

	out = mPendingActions.front();
	mPendingActions.pop();
	return true;
}

unsigned int NetworkNode::getCategory() const
{
	return Category::Network;
}
