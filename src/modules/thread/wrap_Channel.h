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

#ifndef LOVE_THREAD_WRAP_CHANNEL_H
#define LOVE_THREAD_WRAP_CHANNEL_H

// LOVE
#include "Channel.h"

namespace love
{
namespace thread
{
Channel *luax_checkchannel(lua_State *L, int idx);
int w_Channel_push(lua_State *L);
int w_Channel_supply(lua_State *L);
int w_Channel_pop(lua_State *L);
int w_Channel_demand(lua_State *L);
int w_Channel_peek(lua_State *L);
int w_Channel_getCount(lua_State *L);
int w_Channel_clear(lua_State *L);

extern "C" int luaopen_channel(lua_State *L);
} // thread
} // love

#endif // LOVE_THREAD_WRAP_CHANNEL_H
