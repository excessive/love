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

#include "wrap_Thread.h"

namespace love
{
namespace thread
{
namespace sdl
{
	Thread *luax_checkthread(lua_State *L, int idx)
	{
		return luax_checktype<Thread>(L, idx, "Thread", THREAD_THREAD_T);
	}

	int w_Thread_start(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		t->start();
		return 0;
	}

	int w_Thread_kill(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		t->kill();
		return 0;
	}

	int w_Thread_wait(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		t->wait();
		return 0;
	}

	int w_Thread_getName(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		lua_pushstring(L, t->getName().c_str());
		return 1;
	}

	int w_Thread_receive(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		ThreadVariant *v = t->receive(luaL_checkstring(L, 2));
		if (v)
		{
			switch(v->type)
			{
				case BOOLEAN:
					lua_pushboolean(L, v->data.boolean);
					break;
				case NUMBER:
					lua_pushnumber(L, v->data.number);
					break;
				case STRING:
					lua_pushstring(L, v->data.string);
					break;
				case USERDATA: //FIXME: full userdata
					lua_pushlightuserdata(L, v->data.userdata);
					break;
				default:
					lua_pushnil(L);
					break;
			}
		}
		else
			lua_pushnil(L);
		return 1;
	}

	int w_Thread_send(lua_State *L)
	{
		Thread *t = luax_checkthread(L, 1);
		std::string name = luaL_checkstring(L, 2);
		ThreadVariant *v;
		if (lua_isboolean(L, 3))
		{
			v = new ThreadVariant((bool) lua_toboolean(L, 3));
		}
		else if (lua_isnumber(L, 3))
		{
			v = new ThreadVariant(lua_tonumber(L, 3));
		}
		else if (lua_isstring(L, 3))
		{
			v = new ThreadVariant(lua_tostring(L, 3));
		}
		else if (lua_islightuserdata(L, 3))
		{
			v = new ThreadVariant(lua_touserdata(L, 3));
		}
		else
		{
			return luaL_error(L, "Expected boolean, number, string or userdata");
		}
		t->send(name, v);
		v->release();
		return 0;
	}

	static const luaL_Reg type_functions[] = {
		{ "start", w_Thread_start },
		{ "kill", w_Thread_kill },
		{ "wait", w_Thread_wait },
		{ "getName", w_Thread_getName },
		{ "receive", w_Thread_receive },
		{ "send", w_Thread_send },
		{ 0, 0 }
	};

	int luaopen_thread(lua_State *L)
	{
		return luax_register_type(L, "Thread", type_functions);
	}

	static ThreadModule *instance;

	int w_newThread(lua_State *L)
	{
		std::string name = luaL_checkstring(L, 1);
		if (lua_isstring(L, 2))
			luax_convobj(L, 2, "filesystem", "read");
		love::Data *data = luax_checktype<love::Data>(L, 2, "Data", DATA_T);
		Thread *t = instance->newThread(name, data);
		luax_newtype(L, "Thread", THREAD_THREAD_T, (void*)t);
		return 1;
	}

	int w_getThreads(lua_State *L)
	{
		return 0;
	}

	int w_getThread(lua_State *L)
	{
		std::string name = luaL_checkstring(L, 1);
		Thread *t = instance->getThread(name);
		if (t)
		{
			luax_newtype(L, "Thread", THREAD_THREAD_T, (void*)t);
			t->lock();
			t->retain();
			t->unlock();
		}
		else
			lua_pushnil(L);
		return 1;
	}


	// List of functions to wrap.
	static const luaL_Reg module_functions[] = {
		{ "newThread", w_newThread },
		{ "getThread", w_getThread },
		{ 0, 0 }
	};

	static const lua_CFunction types[] = {
		luaopen_thread,
		0
	};

	int luaopen_love_thread(lua_State *L)
	{
		if(instance == 0)
		{
			try
			{
				instance = new ThreadModule();
			}
			catch(Exception & e)
			{
				return luaL_error(L, e.what());
			}
		}
		else
			instance->retain();

		WrappedModule w;
		w.module = instance;
		w.name = "thread";
		w.flags = MODULE_T;
		w.functions = module_functions;
		w.types = types;

		return luax_register_module(L, w);
	}
}
}
}
