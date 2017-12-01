#include <chrono>
#pragma once
class StopWatch
{
public:
	StopWatch(bool startTimer);
	void Start();
	void Stop();
	double StopAndGet();
	double getElapsedTime();
	double StopGetAndReset();
	~StopWatch();
private:
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> end;
	std::chrono::duration<double> elapsed_Seconds;
	double elapsedTime;
};

