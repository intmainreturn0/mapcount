#ifndef MAPCOUNT_TIMER_H
#define MAPCOUNT_TIMER_H

#include <chrono>

class Timer {
	const char * message;
	std::chrono::high_resolution_clock::time_point start;

public:
	Timer( const char * s );

	void stop();
};

#endif //MAPCOUNT_TIMER_H
