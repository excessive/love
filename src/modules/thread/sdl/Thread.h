/**
* Copyright (c) 2006-2010 LOVE Development Team
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

#ifndef LOVE_THREAD_THREAD_H
#define LOVE_THREAD_THREAD_H

// SDL
#include <SDL_thread.h>

// STL
#include <map>
#include <string>

// LOVE
#include <common/Module.h>
#include <filesystem/File.h>
#include <common/runtime.h>

namespace love
{
namespace thread
{
namespace sdl
{
	class ThreadModuleRegistrar
	{
	public:
		virtual void unregister(std::string name) = 0;
	};

	class Thread : public love::Object
	{
	private:
		SDL_Thread *handle;
		ThreadModuleRegistrar *reg;
		std::string name;
		char *data;

	public:
		Thread(ThreadModuleRegistrar *reg, std::string name, love::Data *data);
		~Thread();
		void start();
		void kill();
		std::string getName();
	}; // Thread

	typedef std::map<std::string, Thread*> threadlist_t;

	class ThreadModule : public Module, ThreadModuleRegistrar
	{
	private:
		threadlist_t threads;

	public:
		~ThreadModule();
		Thread *newThread(std::string name, love::Data *data);
		Thread **getThreads();
		Thread *getThread(std::string name);
		void unregister(std::string name);
		const char *getName() const;
	}; // ThreadModule
} // sdl
} // thread
} // love

#endif // LOVE_THREAD_THREAD_H
