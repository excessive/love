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

#include "wrap_ThreadModule.h"
#include "wrap_LuaThread.h"
#include "wrap_Channel.h"
#include <filesystem/File.h>
#include <filesystem/FileData.h>

namespace love
{
namespace thread
{
	static ThreadModule *instance = 0;

	int w_newThread(lua_State *L)
	{
		std::string name = "Thread code";
		love::Data *data;
		if (lua_isstring(L, 1))
			luax_convobj(L, 1, "filesystem", "newFile");
		if (luax_istype(L, 1, FILESYSTEM_FILE_T))
		{
			try
			{
				love::filesystem::File * file = luax_checktype<love::filesystem::File>(L, 1, "File", FILESYSTEM_FILE_T);
				name = std::string("@") + file->getFilename();
				data = file->read();
			}
			catch (love::Exception & e)
			{
				return luaL_error(L, "%s", e.what());
			}
		}
		else if (luax_istype(L, 1, FILESYSTEM_FILE_DATA_T))
		{
			love::filesystem::FileData * fdata = luax_checktype<love::filesystem::FileData>(L, 1, "FileData", FILESYSTEM_FILE_DATA_T);
			name = std::string("@") + fdata->getFilename();
			data = fdata;
			data->retain();
		}
		else
		{
			data = luax_checktype<love::Data>(L, 1, "Data", DATA_T);
			data->retain();
		}
		LuaThread *t = instance->newThread(name, data);
		// do not worry, file->read() returns retained data
		data->release();
		luax_newtype(L, "Thread", THREAD_THREAD_T, (void*)t);
		return 1;
	}

	int w_newChannel(lua_State *L)
	{
		Channel *c = instance->newChannel();
		luax_newtype(L, "Channel", THREAD_CHANNEL_T, (void*)c);
		return 1;
	}

	int w_getChannel(lua_State *L)
	{
		std::string name = luax_checkstring(L, 1);
		Channel *c = instance->getChannel(name);
		luax_newtype(L, "Channel", THREAD_CHANNEL_T, (void*)c);
		return 1;
	}

	// List of functions to wrap.
	static const luaL_Reg module_functions[] = {
		{ "newThread", w_newThread },
		{ "newChannel", w_newChannel },
		{ "getChannel", w_getChannel },
		{ 0, 0 }
	};

	static const lua_CFunction types[] = {
		luaopen_thread,
		luaopen_channel,
		0
	};

	extern "C" int luaopen_love_thread(lua_State *L)
	{
		if (instance == 0)
		{
			try
			{
				instance = new ThreadModule();
			}
			catch (Exception & e)
			{
				return luaL_error(L, "%s", e.what());
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
