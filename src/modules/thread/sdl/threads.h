/**
 * Copyright (c) 2006-2012 LOVE Development Team
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

#ifndef LOVE_THREAD_SDL_THREADS_H
#define LOVE_THREAD_SDL_THREADS_H

#include <common/config.h>
#include <thread/threads.h>

#include <SDL_thread.h>

namespace love
{
namespace thread
{
namespace sdl
{
class Conditional;

class Mutex : public thread::Mutex
{
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	SDL_mutex *mutex;
	Mutex(const Mutex&/* mutex*/) {}

	friend class Conditional;
};

class Conditional : public thread::Conditional
{
public:
	Conditional();
	~Conditional();

	void signal();
	void broadcast();
	bool wait(thread::Mutex *mutex, int timeout=-1);

private:
	SDL_cond *cond;
};

} // sdl
} // thread
} // love

#endif /* LOVE_THREAD_SDL_THREADS_H */
