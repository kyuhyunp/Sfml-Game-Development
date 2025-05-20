#ifndef PARALLEL_TASK_H
#define PARALLEL_TASK_H

#include <SFML/System/Clock.hpp>

#include <thread>
#include <mutex>


class ParallelTask
{
public:
	ParallelTask();

	void execute();
	bool isFinished();
	float getCompletion();

private:
	void runTask();

	std::thread mThread;
	bool mFinished;
	sf::Clock mElapsedTime;
	std::mutex mMutex;
};

#endif
