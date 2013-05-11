/**
 * Copyright (c) 2006-2013 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include "common/config.h"
#include "Timer.h"

#include "common/delay.h"
#include "common/int.h"

// SDL
#include <SDL.h>

#if defined(LOVE_WINDOWS)
#include <windows.h>
#elif defined(LOVE_MACOSX)
#include <mach/mach_time.h>
#include <sys/time.h>
#elif defined(LOVE_LINUX)
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#endif

namespace
{
#if defined(LOVE_LINUX)
inline double getTimeOfDay()
{
	timeval t;
	gettimeofday(&t, NULL);
	return (double) t.tv_sec + (double) t.tv_usec / 1000000.0;
}
#endif
}

namespace love
{
namespace timer
{
namespace sdl
{

Timer::Timer()
	: currTime(0)
	, prevFpsUpdate(0)
	, fps(0)
	, averageDelta(0)
	, fpsUpdateFrequency(1)
	, frames(0)
	, dt(0)
	, timerPeriod(getTimerPeriod())
{
	// Init the SDL timer system (needed for SDL_Delay.)
	if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0)
		throw love::Exception("%s", SDL_GetError());

	prevFpsUpdate = currTime = getTime();
}

Timer::~Timer()
{
	// Quit SDL timer.
	SDL_QuitSubSystem(SDL_INIT_TIMER);
}

const char *Timer::getName() const
{
	return "love.timer.sdl";
}

void Timer::step()
{
	// Frames rendered
	frames++;

	// "Current" time is previous time by now.
	prevTime = currTime;

	// Get time from system.
	currTime = getTime();

	// Convert to number of seconds.
	dt = currTime - prevTime;

	double timeSinceLast = currTime - prevFpsUpdate;
	// Update FPS?
	if (timeSinceLast > fpsUpdateFrequency)
	{
		fps = int((frames/timeSinceLast) + 0.5);
		averageDelta = timeSinceLast/frames;
		prevFpsUpdate = currTime;
		frames = 0;
	}
}

void Timer::sleep(double seconds) const
{
	if (seconds > 0)
		delay((int)(seconds*1000));
}

double Timer::getDelta() const
{
	return dt;
}

int Timer::getFPS() const
{
	return fps;
}

double Timer::getAverageDelta() const
{
	return averageDelta;
}

double Timer::getTimerPeriod()
{
#if defined(LOVE_MACOSX)
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	return (double) info.numer / (double) info.denom / 1000000000.0;
#elif defined(LOVE_WINDOWS)
	LARGE_INTEGER temp;
	if (QueryPerformanceFrequency(&temp) != 0 && temp.QuadPart != 0)
		return 1.0 / (double) temp.QuadPart;
#endif
	return 0;
}

double Timer::getTime() const
{
#if defined(LOVE_LINUX)
	double mt;
	// Check for POSIX timers and monotonic clocks. If not supported, use the gettimeofday fallback.
#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK) \
&& (defined(CLOCK_MONOTONIC_RAW) || defined(CLOCK_MONOTONIC))
	timespec t;
#ifdef CLOCK_MONOTONIC_RAW
	clockid_t clk_id = CLOCK_MONOTONIC_RAW;
#else
	clockid_t clk_id = CLOCK_MONOTONIC;
#endif
	if (clock_gettime(clk_id, &t) == 0)
		mt = (double) t.tv_sec + (double) t.tv_nsec / 1000000000.0;
	else
#endif
		mt = getTimeOfDay();
	return mt;
#elif defined(LOVE_MACOSX)
	return (double) mach_absolute_time() * timerPeriod;
#elif defined(LOVE_WINDOWS)
	LARGE_INTEGER microTime;
	QueryPerformanceCounter(&microTime);
	return (double) microTime.QuadPart * timerPeriod;
#endif
}

} // sdl
} // timer
} // love
