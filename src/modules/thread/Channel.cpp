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

#include "Channel.h"
#include <map>
#include <string>

namespace love
{
namespace thread
{
	static std::map<std::string, Channel*> namedChannels;
	static Mutex *namedChannelMutex;
	Channel *Channel::getChannel(const std::string &name)
	{
		if (!namedChannelMutex)
			namedChannelMutex = newMutex();
		Lock l(namedChannelMutex);
		if (!namedChannels.count(name))
			namedChannels[name] = new Channel(name);
		else
			namedChannels[name]->retain();
		return namedChannels[name];
	}

	Channel::Channel()
		: named(false)
	{
		mutex = newMutex();
		cond = newConditional();
	}

	Channel::Channel(const std::string &name)
		: named(true), name(name)
	{
		mutex = newMutex();
		cond = newConditional();
	}

	Channel::~Channel()
	{
		while (!queue.empty())
		{
			queue.front()->release();
			queue.pop();
		}

		delete mutex;
		delete cond;
		if (named)
			namedChannels.erase(name);
	}

	void Channel::push(Variant *var)
	{
		if (!var)
			return;
		Lock l(mutex);
		var->retain();
		// Keep a reference to ourselves
		// if we're non-empty and named.
		if (named && queue.empty())
			retain();
		queue.push(var);
		cond->signal();
	}

	Variant *Channel::pop()
	{
		Lock l(mutex);
		if (queue.empty())
			return 0;

		Variant *var = queue.front();
		queue.pop();

		// Release our reference to ourselves
		// if we're empty and named.
		if (named && queue.empty())
			release();
		return var;
	} // NOTE: Returns a retained Variant

	Variant *Channel::demand()
	{
		Variant *var;
		while (!(var = pop()))
		{
			mutex->lock();
			cond->wait(mutex);
			mutex->unlock();
		}
		return var;
	}

	Variant *Channel::peek()
	{
		Lock l(mutex);
		if (queue.empty())
			return 0;

		Variant *var = queue.front();
		var->retain();
		return var;
	}

	int Channel::count()
	{
		Lock l(mutex);
		return queue.size();
	}

	void Channel::clear()
	{
		Lock l(mutex);

		// We're already empty.
		if (queue.empty())
			return;

		while (!queue.empty())
		{
			queue.front()->release();
			queue.pop();
		}

		// Once again, release our own
		// reference if we're named.
		if (named)
			release();
	}

	void Channel::release()
	{
		if (named)
			namedChannelMutex->lock();

		Object::release();

		if (named)
			namedChannelMutex->unlock();
	}
} // thread
} // love
