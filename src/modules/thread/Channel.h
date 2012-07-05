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

#ifndef LOVE_THREAD_CHANNEL_H
#define LOVE_THREAD_CHANNEL_H

// STL
#include <queue>
#include <string>

// LOVE
#include <common/Variant.h>
#include <thread/threads.h>

namespace love
{
namespace thread
{
class Channel : public love::Object
{
private:
	Mutex *mutex;
	Conditional *cond;
	std::queue<Variant*> queue;
	bool named;
	std::string name;
	Channel(const std::string &name);

public:
	Channel();
	~Channel();
	static Channel *getChannel(const std::string &name);

	void push(Variant *var);
	Variant *pop();
	Variant *demand();
	Variant *peek();
	int count();
	void clear();

	void retain();
	void release();
}; // Channel
} // thread
} // love

#endif // LOVE_THREAD_CHANNEL_H
