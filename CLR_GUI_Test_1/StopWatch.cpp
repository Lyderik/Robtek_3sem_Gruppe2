#include "StopWatch.h"



StopWatch::StopWatch(bool startTimer)
{
	if (startTimer)
	{
		Start();
	}
}

void StopWatch::Start()
{
	start = std::chrono::system_clock::now();
}

void StopWatch::Stop()
{
	end = std::chrono::system_clock::now();
	elapsed_Seconds = end - start;
	elapsedTime = elapsed_Seconds.count();
}

double StopWatch::StopAndGet()
{
	Stop();
	return elapsedTime;
}

double StopWatch::getElapsedTime()
{
	return elapsedTime;
}

double StopWatch::StopGetAndReset()
{
	double d = StopAndGet();
	Start();
	return d;
}


StopWatch::~StopWatch()
{
}
