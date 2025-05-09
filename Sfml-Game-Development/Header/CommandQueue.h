#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include "Command.h"

#include <cassert>
#include <queue>

class CommandQueue
{
public:
	CommandQueue();

	void push(const Command& command);
	Command pop();
	bool isEmpty() const;

private:
	std::queue<Command> mQueue;

};
#endif
