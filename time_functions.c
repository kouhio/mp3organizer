#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#ifdef __cplusplus
extern "C" 
{
#endif

#if (OS_LINUX == 0)
#include <dos.h>
#include <dir.h>
#include <unistd.h>
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

unsigned long long timer_functions::TickCount(void)
{
	unsigned long long dw = 0;

	struct timeval tvnow;
	unsigned long long s,us;

	gettimeofday(&tvnow, NULL);
	s= (unsigned long long)tvnow.tv_sec;
	us= (unsigned long long)tvnow.tv_usec;

	dw = (unsigned long long)((s * (unsigned long long)1000) +(us / (unsigned long long)1000));

	return (unsigned long long)dw;
}

void timer_functions::delay_ms(unsigned long delay_time)
{
	unsigned long long DelayTime = TickCount();
	unsigned long long tick_time = DelayTime;

	while ((unsigned long long)tick_time < (unsigned long long)((unsigned long long)(DelayTime + (unsigned long long)delay_time)))
	{
#if (OS_LINUX == 1)
		sched_yield();
#endif
		tick_time = TickCount();
	}
}

void timer_functions::reset_timers(void)
{
	start_time = TickCount();
}

unsigned long long timer_functions::get_time_passed(void)
{
	current_time = TickCount();
	return (current_time - start_time);
}

