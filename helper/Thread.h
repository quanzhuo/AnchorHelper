/*! ------------------------------------------------------------------------------------------------------------------
* @file thread.h
* @brief
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#ifndef _THREAD_H_
#define _THREAD_H_

/*
 * Provides a C++ Abstraction of threads.
 */
#include <pthread.h>
#include <stddef.h>
#include <sys/timeb.h>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#endif

class Thread
{
public:
	Thread()
	{
#ifdef _WIN32
		timer = CreateWaitableTimer(NULL, TRUE, NULL);
#endif
	}
	virtual ~Thread() {}

	/** Returns true if the thread was successfully started,
	false if there was an error starting the thread
	*/
	bool Start()
	{
		return (pthread_create(&_thread, NULL, _Run, this) == 0);
	}

	/** Will not return until the internal thread has exited. */
	void WaitToExit()
	{
		(void)pthread_join(_thread, NULL);
	}

	uint64_t now()
	{
		struct timeb tp;
		ftime(&tp);
		return tp.time * 1000 + tp.millitm;
	}

	void sleep(uint64_t dly)
	{
#ifdef _WIN32
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -10 * dly;
		SetWaitableTimer(timer, &liDueTime, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
#else
		struct timespec t;
		t.tv_sec = dly / 1000;
		t.tv_nsec = (dly % 1000) * 1000000;

		while (nanosleep(&t, &t) == -1) {}
#endif
	}

protected:
	/** Implement this method in your subclass
	with the code you want your thread to run.
	*/
	virtual void Run() = 0;

#ifdef _WIN32
	HANDLE timer;
#endif

private:
	static void * _Run(void * This) { ((Thread *)This)->Run(); return NULL; }

	pthread_t _thread;
};

#ifdef _WIN32
#define sleep(x) Sleep((x*1000))
#define pthread_yield sched_yield
#endif

#if (defined(__MACH__) && defined(__APPLE__))
#define pthread_yield pthread_yield_np
#endif

#endif // _THREAD_H_
