/**
 * Copyright (c) 2006-2018 LOVE Development Team
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

#include "Channel.h"
#include <map>
#include <string>

#include <timer/Timer.h>

namespace love
{
namespace thread
{

love::Type Channel::type("Channel", &Object::type);
static std::map<std::string, Channel *> namedChannels;
static Mutex *namedChannelMutex;

Channel *Channel::getChannel(const std::string &name)
{
	if (!namedChannelMutex)
		namedChannelMutex = newMutex();

	Lock lock(namedChannelMutex);

	auto it = namedChannels.find(name);
	if (it != namedChannels.end())
	{
		it->second->retain();
		return it->second;
	}

	namedChannels[name] = new Channel(name);
	return namedChannels[name];
}

Channel::Channel()
	: named(false)
	, sent(0)
	, received(0)
{
}

Channel::Channel(const std::string &name)
	: named(true)
	, name(name)
	, sent(0)
	, received(0)
{
}

Channel::~Channel()
{
	if (named)
	{
		Lock l(namedChannelMutex);
		namedChannels.erase(name);
	}
}

uint64 Channel::push(const Variant &var)
{
	Lock l(mutex);

	// Keep a reference to ourselves
	// if we're non-empty and named.
	if (named && queue.empty())
		retain();

	queue.push(var);
	cond->broadcast();

	return ++sent;
}

bool Channel::supply(const Variant &var)
{
	Lock l(mutex);
	uint64 id = push(var);

	while (received < id)
		cond->wait(mutex);

	return true;
}

bool Channel::supply(const Variant &var, double timeout)
{
	Lock l(mutex);
	uint64 id = push(var);

	while (timeout >= 0)
	{
		if (received >= id)
			return true;

		double start = love::timer::Timer::getTime();
		cond->wait(mutex, timeout*1000);
		double stop = love::timer::Timer::getTime();

		timeout -= (stop-start);
	}

	return false;
}

bool Channel::pop(Variant *var)
{
	Lock l(mutex);

	if (queue.empty())
		return false;

	*var = queue.front();
	queue.pop();

	received++;
	cond->broadcast();

	// Release our reference to ourselves
	// if we're empty and named.
	if (named && queue.empty())
		release();

	return true;
}

bool Channel::demand(Variant *var)
{
	Lock l(mutex);

	while (!pop(var))
		cond->wait(mutex);

	return true;
}

bool Channel::demand(Variant *var, double timeout)
{
	Lock l(mutex);

	while (timeout >= 0)
	{
		if (pop(var))
			return true;

		double start = love::timer::Timer::getTime();
		cond->wait(mutex, timeout*1000);
		double stop = love::timer::Timer::getTime();

		timeout -= (stop-start);
	}

	return false;
}

bool Channel::peek(Variant *var)
{
	Lock l(mutex);

	if (queue.empty())
		return false;

	*var = queue.front();
	return true;
}

int Channel::getCount() const
{
	Lock l(mutex);
	return (int) queue.size();
}

bool Channel::hasRead(uint64 id) const
{
	Lock l(mutex);
	return received >= id;
}

void Channel::clear()
{
	Lock l(mutex);

	// We're already empty.
	if (queue.empty())
		return;

	while (!queue.empty())
		queue.pop();

	// Finish all the supply waits
	received = sent;
	cond->broadcast();

	// Once again, release our own
	// reference if we're named.
	if (named)
		release();
}

void Channel::lockMutex()
{
	mutex->lock();
}

void Channel::unlockMutex()
{
	mutex->unlock();
}

} // thread
} // love
