#include "../../Header/Learn/ParallelTask.h"
#include <SFML/System/Time.hpp>


ParallelTask::ParallelTask()
	: mThread(&ParallelTask::runTask, this)
	, mFinished(false)
	, mElapsedTime()
	, mMutex()
{
}

void ParallelTask::execute()
{
	mFinished = false;
	mElapsedTime.restart();
	mThread.detach();
}

bool ParallelTask::isFinished()
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mFinished;
}

float ParallelTask::getCompletion()
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mElapsedTime.getElapsedTime().asSeconds() / 10.f;
}

void ParallelTask::runTask()
{
	bool ended = false;
	while (!ended)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		if (mElapsedTime.getElapsedTime().asSeconds() >= 10.f)
		{
			ended = true;
		}
	}

	{
		std::lock_guard<std::mutex> lock(mMutex);
		mFinished = true;
	}
}
