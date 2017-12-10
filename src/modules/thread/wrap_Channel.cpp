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
	return luax_checktype<Channel>(L, idx);
}

int w_Channel_push(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	luax_catchexcept(L, [&]() {
		Variant var = Variant::fromLua(L, 2);
		if (var.getType() == Variant::UNKNOWN)
			luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");
		uint64 id = c->push(var);
		lua_pushnumber(L, (lua_Number) id);
	});
	return 1;
}

int w_Channel_supply(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	bool result = false;
	luax_catchexcept(L, [&]() {
		Variant var = Variant::fromLua(L, 2);
		if (var.getType() == Variant::UNKNOWN)
			luaL_argerror(L, 2, "boolean, number, string, love type, or table expected");
		if (lua_isnumber(L, 3))
			result = c->supply(var, lua_tonumber(L, 3));
		else
			result = c->supply(var);
	});

	luax_pushboolean(L, result);
	return 1;
}

int w_Channel_pop(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	Variant var;
	if (c->pop(&var))
		var.toLua(L);
	else
		lua_pushnil(L);
	return 1;
}

int w_Channel_demand(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	Variant var;
	bool result = false;

	if (lua_isnumber(L, 2))
		result = c->demand(&var, lua_tonumber(L, 2));
	else
		result = c->demand(&var);

	if (result)
		var.toLua(L);
	else
		lua_pushnil(L);
	return 1;
}

int w_Channel_peek(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	Variant var;
	if (c->peek(&var))
		var.toLua(L);
	else
		lua_pushnil(L);
	return 1;
}

int w_Channel_getCount(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	lua_pushnumber(L, c->getCount());
	return 1;
}

int w_Channel_hasRead(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	uint64 id = (uint64) luaL_checknumber(L, 2);
	luax_pushboolean(L, c->hasRead(id));
	return 1;
}

int w_Channel_clear(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	c->clear();
	return 0;
}

int w_Channel_performAtomic(lua_State *L)
{
	Channel *c = luax_checkchannel(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);

	// Pass this channel as an argument to the function.
	lua_pushvalue(L, 1);
	lua_insert(L, 3);

	c->lockMutex();

	// call the function, passing the channel as the first argument and any
	// user-specified arguments after.
	int numargs = lua_gettop(L) - 2;
	int err = lua_pcall(L, numargs, LUA_MULTRET, 0);

	c->unlockMutex();

	// Unfortunately, this eats the stack trace, too bad.
	if (err != 0)
		return lua_error(L);

	// The function and everything after it in the stack are eaten by the pcall,
	// leaving only the Channel argument. Everything else is a return value.
	return lua_gettop(L) - 1;
}

static const luaL_Reg w_Channel_functions[] =
{
	{ "push", w_Channel_push },
	{ "supply", w_Channel_supply },
	{ "pop", w_Channel_pop },
	{ "demand", w_Channel_demand },
	{ "peek", w_Channel_peek },
	{ "getCount", w_Channel_getCount },
	{ "hasRead", w_Channel_hasRead },
	{ "clear", w_Channel_clear },
	{ "performAtomic", w_Channel_performAtomic },
	{ 0, 0 }
};

extern "C" int luaopen_channel(lua_State *L)
{
	return luax_register_type(L, &Channel::type, w_Channel_functions, nullptr);
}

} // thread
} // love
