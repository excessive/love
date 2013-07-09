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

#ifndef LOVE_WINDOW_WRAP_WINDOW_H
#define LOVE_WINDOW_WRAP_WINDOW_H

#include "common/config.h"
#include "common/runtime.h"

namespace love
{
namespace window
{

int w_checkMode(lua_State *L);
int w_setMode(lua_State *L);
int w_getMode(lua_State *L);
int w_getModes(lua_State *L);
int w_toggleFullscreen(lua_State *L);
int w_isCreated(lua_State *L);
int w_getWidth(lua_State *L);
int w_getHeight(lua_State *L);
int w_getDimensions(lua_State *L);
int w_setIcon(lua_State *L);
int w_getIcon(lua_State *L);
int w_setCaption(lua_State *L);
int w_getCaption(lua_State *L);
int w_hasFocus(lua_State *L);
int w_hasMouseFocus(lua_State *L);
int w_isVisible(lua_State *L);
extern "C" LOVE_EXPORT int luaopen_love_window(lua_State *L);

} // window
} // love

#endif // LOVE_WINDOW_WRAP_WINDOW_H
