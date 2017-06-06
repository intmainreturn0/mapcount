#include <cstdio>
#include "timer.h"

using namespace std::chrono;

#ifdef NO_DEBUG_INFO

Timer::Timer( const char *s ) {}
void Timer::stop() {}

#else

Timer::Timer( const char *s ) : message( s ) {
    start = high_resolution_clock::now();
}

void Timer::stop() {
    high_resolution_clock::time_point end = high_resolution_clock::now();
    long duration = duration_cast<milliseconds>( end - start ).count();
    printf( "%s: %ld ms\n", message, duration );
}

#endif