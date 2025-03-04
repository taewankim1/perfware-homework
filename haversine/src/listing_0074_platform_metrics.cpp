#include <utils.h>
#include <stdio.h>

#include <sys/time.h>

static u64 GetOSTimerFreq(void)
{
	return 1000000; // Microseconds per second
}

static u64 ReadOSTimer(void)
{
	// NOTE(casey): The "struct" keyword is not necessary here when compiling in C++,
	// but just in case anyone is using this file from C, I include it.
	struct timeval Value;
	gettimeofday(&Value, 0);
	
	u64 Result = GetOSTimerFreq()*(u64)Value.tv_sec + (u64)Value.tv_usec;
	return Result;
}

// Read the ARMv8 timer (Apple Silicon)
inline u64 ReadCPUTimer(void) {
    u64 value;
    asm volatile("mrs %0, cntvct_el0" : "=r"(value));
    return value;
}

// Get the timer frequency
inline u64 GetCPUtimerFreq(void) {
    // OSFreq = OS cycles in a second
    // OSElapsed - OS cycles in the given seconds
    // OSFreq / OSElapsed = the given seconds / one second
    // OSFreq / OSElapsed * CPUElapsed = CPU cycles in a second
    u64 CPUFreq = 0;

    u64 MillisecondsToWait = 1000;

	u64 OSFreq = GetOSTimerFreq();
	u64 CPUStart = ReadCPUTimer();
	u64 OSStart = ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
	while(OSElapsed < OSWaitTime)
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}
	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;

	if(OSElapsed)
	{
		CPUFreq = OSFreq * CPUElapsed / OSElapsed;
	}

    return CPUFreq;
}