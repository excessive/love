/**
 * Copyright (c) 2006-2018 LOVE Development Team
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

#pragma once

// LOVE
#include "common/config.h"
#include "common/runtime.h"
#include "Mesh.h"

namespace love
{
namespace graphics
{

char *luax_writeAttributeData(lua_State *L, int startidx, vertex::DataType type, int components, char *data);
const char *luax_readAttributeData(lua_State *L, vertex::DataType type, int components, const char *data);

Mesh *luax_checkmesh(lua_State *L, int idx);
extern "C" int luaopen_mesh(lua_State *L);

} // graphics
} // love
