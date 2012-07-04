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

#include "wrap_Channel.h"

namespace love
{
namespace thread
{
	Channel *luax_checkchannel(lua_State *L, int idx)
	{
		return luax_checktype<Channel>(L, idx, "Channel", THREAD_CHANNEL_T);
	}

	int w_Channel_push(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		Variant *var = Variant::fromLua(L, 2);
		c->push(var);
		var->release();
		return 0;
	}

	int w_Channel_pop(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		Variant *var = c->pop();
		if (var)
		{
			var->toLua(L);
			var->release();
		}
		else
			lua_pushnil(L);
		return 1;
	}

	int w_Channel_demand(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		Variant *var = c->demand();
		var->toLua(L);
		var->release();
		return 1;
	}

	int w_Channel_peek(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		Variant *var = c->peek();
		if (var)
		{
			var->toLua(L);
			var->release();
		}
		else
			lua_pushnil(L);
		return 1;
	}

	int w_Channel_count(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		lua_pushnumber(L, c->count());
		return 1;
	}

	int w_Channel_clear(lua_State *L)
	{
		Channel *c = luax_checkchannel(L, 1);
		c->clear();
		return 0;
	}

	static const luaL_Reg type_functions[] = {
		{ "push", w_Channel_push },
		{ "pop", w_Channel_pop },
		{ "demand", w_Channel_demand },
		{ "peek", w_Channel_peek },
		{ "count", w_Channel_count },
		{ "clear", w_Channel_clear },
		{ 0, 0 }
	};

	extern "C" int luaopen_channel(lua_State *L)
	{
		return luax_register_type(L, "Channel", type_functions);
	}
}
}
