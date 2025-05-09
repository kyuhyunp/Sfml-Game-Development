#include "../Header/CommandQueue.h"

CommandQueue::CommandQueue()
{
}

void CommandQueue::push(const Command& command)
{
	mQueue.push(command);
}


Command CommandQueue::pop()
{
	assert(!isEmpty());
	
	Command ret = mQueue.front();
	mQueue.pop();
	return ret;
}

bool CommandQueue::isEmpty() const
{
	return mQueue.empty();
}