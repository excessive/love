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

#include <common/config.h>

#include "Thread.h"

#ifdef LOVE_BUILD_STANDALONE
extern "C" int luaopen_love(lua_State * L);
#endif // LOVE_BUILD_STANDALONE
extern "C" int luaopen_love_thread(lua_State *L);

namespace love
{
namespace thread
{
namespace sdl
{
	int threadfunc(ThreadData *comm)
	{
		lua_State * L = lua_open();
		luaL_openlibs(L);
	#ifdef LOVE_BUILD_STANDALONE
		love::luax_preload(L, luaopen_love, "love");
		luaopen_love(L);
	#endif // LOVE_BUILD_STANDALONE
		luaopen_love_thread(L);
		lua_pushstring(L, comm->getName());
		luax_convobj(L, lua_gettop(L), "thread", "getThread");
		lua_setglobal(L, "thread");
		if(luaL_dostring(L, comm->getCode()) == 1)
		{
			ThreadVariant *v = new ThreadVariant(lua_tostring(L, -1));
			comm->setValue("error", v);
			v->release();
		}
		lua_close(L);
		return 0;
	}

	ThreadVariant::ThreadVariant(bool boolean)
	{
		type = BOOLEAN;
		data.boolean = boolean;
	}

	ThreadVariant::ThreadVariant(double number)
	{
		type = NUMBER;
		data.number = number;
	}

	ThreadVariant::ThreadVariant(const char *string)
	{
		type = STRING;
		size_t len = strlen(string);
		char *buf = new char[len+1];
		memset(buf, 0, len+1);
		memcpy(buf, string, len);
		data.string = buf;
	}

	ThreadVariant::ThreadVariant(void *userdata)
	{
		type = LUSERDATA;
		data.userdata = userdata;
	}

	ThreadVariant::ThreadVariant(Type udatatype, void *userdata)
	{
		type = FUSERDATA;
		this->udatatype = udatatype;
		Proxy *p = (Proxy *) userdata;
		flags = p->flags;
		data.userdata = p->data;
		((love::Object *) data.userdata)->retain();
	}

	ThreadVariant::~ThreadVariant()
	{
		switch(type)
		{
			case STRING:
				delete[] data.string;
				break;
			case FUSERDATA:
				((love::Object *) data.userdata)->release();
				break;
			default:
				break;
		}
	}

	ThreadData::ThreadData(const char *name, const char *code)
	{
		size_t len = strlen(name);
		this->name = new char[len+1];
		memset(this->name, 0, len+1);
		memcpy(this->name, name, len);
		if (code)
		{
			len = strlen(code);
			this->code = new char[len+1];
			memset(this->code, 0, len+1);
			memcpy(this->code, code, len);
		}
		else
			this->code = 0;
	}

	ThreadData::~ThreadData()
	{
		delete[] name;
		delete[] code;
	}

	const char *ThreadData::getCode()
	{
		return code;
	}

	const char *ThreadData::getName()
	{
		return name;
	}

	ThreadVariant* ThreadData::getValue(std::string name)
	{
		if (shared.count(name) == 0)
			return 0;
		return shared[name];
	}

	void ThreadData::clearValue(std::string name)
	{
		if (shared.count(name) == 0)
			return;
		shared[name]->release();
		shared.erase(name);
	}

	void ThreadData::setValue(std::string name, ThreadVariant *v)
	{
		if (shared.count(name) != 0)
			shared[name]->release();
		v->retain();
		shared[name] = v;
	}

	Thread::Thread(love::thread::ThreadModule *module, std::string name, love::Data *data)
		: handle(0), module(module), name(name), isThread(true)
	{
		module->retain();
		unsigned int len = data->getSize();
		this->data = new char[len+1];
		memset(this->data, 0, len+1);
		memcpy(this->data, data->getData(), len);
		comm = new ThreadData(name.c_str(), this->data);
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();
	}

	Thread::Thread(love::thread::ThreadModule *module, std::string name)
		: handle(0), module(module), name(name), data(0), isThread(false)
	{
		module->retain();
		comm = new ThreadData(name.c_str(), NULL);
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();
	}

	Thread::~Thread()
	{
		if (data)
			delete[] data;
		delete comm;
		if (handle)
			SDL_KillThread(handle);
		module->unregister(name);
		SDL_DestroyMutex(mutex);
		SDL_DestroyCond(cond);
		module->release();
	}

	void Thread::start()
	{
		if (!handle && isThread)
			handle = SDL_CreateThread((int (*)(void*)) threadfunc, (void*) comm);
	}

	void Thread::kill()
	{
		if (handle)
		{
			SDL_KillThread(handle);
			handle = 0;
		}
	}

	void Thread::wait()
	{
		if (handle)
		{
			SDL_WaitThread(handle, NULL);
			handle = 0;
		}
	}

	void Thread::lock()
	{
		SDL_mutexP(mutex);
	}

	void Thread::unlock()
	{
		SDL_mutexV(mutex);
	}

	std::string Thread::getName()
	{
		return name;
	}

	ThreadVariant *Thread::receive(std::string name)
	{
		lock();
		ThreadVariant *v = comm->getValue(name);
		unlock();
		return v;
	}

	ThreadVariant *Thread::demand(std::string name)
	{
		lock();
		ThreadVariant *v = comm->getValue(name);
		while (!v)
		{
			if (comm->getValue("error"))
				return 0;
			SDL_CondWait(cond, mutex);
			v = comm->getValue(name);
		}
		unlock();
		return v;
	}

	void Thread::clear(std::string name)
	{
		lock();
		comm->clearValue(name);
		unlock();
	}

	void Thread::send(std::string name, ThreadVariant *v)
	{
		lock();
		comm->setValue(name, v);
		unlock();
		SDL_CondBroadcast(cond);
	}

	ThreadModule::ThreadModule()
	{
		threads["main"] = new Thread(this, "main");
	}

	ThreadModule::~ThreadModule()
	{
		for (threadlist_t::iterator i = threads.begin(); i != threads.end(); i++)
		{
			i->second->kill();
		}
	}

	Thread *ThreadModule::newThread(std::string name, love::Data *data)
	{
		if (threads.count(name) != 0)
			return 0;
		Thread *t = new Thread(this, name, data);
		threads[name] = t;
		return t;
	}

	Thread *ThreadModule::getThread(std::string name)
	{
		if (threads.count(name) == 0)
			return 0;
		threadlist_t::iterator i = threads.find(name);
		return i->second;
	}

	Thread **ThreadModule::getThreads()
	{
		Thread **list = new Thread*[threads.size()+1];
		int c = 0;
		for (threadlist_t::iterator i = threads.begin(); i != threads.end(); i++, c++)
		{
			list[c] = i->second;
		}
		list[threads.size()] = 0;
		return list;
	}

	void ThreadModule::unregister(std::string name)
	{
		if (threads.count(name) == 0)
			return;
		threadlist_t::iterator i = threads.find(name);
		threads.erase(i);
	}

	const char *ThreadModule::getName() const
	{
		return "love.thread.sdl";
	}
} // sdl
} // thread
} // love
