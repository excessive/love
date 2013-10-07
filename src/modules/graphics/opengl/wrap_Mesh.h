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

#ifndef LOVE_GRAPHICS_OPENGL_WRAP_MESH_H
#define LOVE_GRAPHICS_OPENGL_WRAP_MESH_H

// LOVE
#include "common/runtime.h"
#include "Mesh.h"

namespace love
{
namespace graphics
{
namespace opengl
{

Mesh *luax_checkmesh(lua_State *L, int idx);

int w_Mesh_setVertex(lua_State *L);
int w_Mesh_getVertex(lua_State *L);
int w_Mesh_setVertices(lua_State *L);
int w_Mesh_getVertexCount(lua_State *L);
int w_Mesh_setVertexMap(lua_State *L);
int w_Mesh_getVertexMap(lua_State *L);
int w_Mesh_setImage(lua_State *L);
int w_Mesh_getImage(lua_State *L);
int w_Mesh_setDrawMode(lua_State *L);
int w_Mesh_getDrawMode(lua_State *L);
int w_Mesh_setVertexColors(lua_State *L);
int w_Mesh_hasVertexColors(lua_State *L);

extern "C" int luaopen_mesh(lua_State *L);

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_WRAP_MESH_H
