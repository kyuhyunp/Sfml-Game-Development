#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#include "SceneNode.h"
#include "NetworkProtocol.h"

#include <queue>

class NetworkNode : public SceneNode
{
public:
	NetworkNode();

	void notifyGameAction(GameActions::Type type, sf::Vector2f position);
	bool pollGameAction(GameActions::Action& out);

	virtual unsigned int getCategory() const;

private:
	std::queue<GameActions::Action> mPendingActions;
};

#endif

