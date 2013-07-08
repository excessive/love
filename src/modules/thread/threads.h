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

#ifndef LOVE_THREAD_THREADS_H
#define LOVE_THREAD_THREADS_H

#include <common/config.h>
#include "Thread.h"

namespace love
{
namespace thread
{

class Mutex
{
public:
	virtual ~Mutex() {}

	virtual void lock() = 0;
	virtual void unlock() = 0;
};

class Conditional
{
public:
	virtual ~Conditional() {}

	virtual void signal() = 0;
	virtual void broadcast() = 0;
	virtual bool wait(Mutex *mutex, int timeout=-1) = 0;
};

class Lock
{
public:
	Lock(Mutex *m);
	Lock(Mutex &m);
	~Lock();

private:
	Mutex *mutex;
};

class EmptyLock
{
public:
	EmptyLock();
	~EmptyLock();

	void setLock(Mutex *m);
	void setLock(Mutex &m);

private:
	Mutex *mutex;
};

class Threadable
{
public:
	Threadable();
	virtual ~Threadable();

	virtual void threadFunction() = 0;

	bool start();
	void wait();

protected:
	Thread *owner;
};

Mutex *newMutex();
Conditional *newConditional();
Thread *newThread(Threadable *t);

} // thread
} // love

#endif /* LOVE_THREAD_THREADS_H */
