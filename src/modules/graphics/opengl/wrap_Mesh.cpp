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

// LOVE
#include "wrap_Mesh.h"
#include "wrap_Image.h"

namespace love
{
namespace graphics
{
namespace opengl
{

Mesh *luax_checkmesh(lua_State *L, int idx)
{
	return luax_checktype<Mesh>(L, idx, "Mesh", GRAPHICS_MESH_T);
}

int w_Mesh_setVertex(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	size_t i = size_t(luaL_checkinteger(L, 2) - 1);

	Vertex v;

	if (lua_istable(L, 3))
	{
		for (int i = 1; i <= 8; i++)
			lua_rawgeti(L, 3, i);

		v.x = luaL_checknumber(L, -8);
		v.y = luaL_checknumber(L, -7);
		v.s = luaL_checknumber(L, -6);
		v.t = luaL_checknumber(L, -5);
		v.r = luaL_optinteger(L, -4, 255);
		v.g = luaL_optinteger(L, -3, 255);
		v.b = luaL_optinteger(L, -2, 255);
		v.a = luaL_optinteger(L, -1, 255);

		lua_pop(L, 8);
	}
	else
	{
		v.x = luaL_checknumber(L, 3);
		v.y = luaL_checknumber(L, 4);
		v.s = luaL_checknumber(L, 5);
		v.t = luaL_checknumber(L, 6);
		v.r = luaL_optinteger(L,  7, 255);
		v.g = luaL_optinteger(L,  8, 255);
		v.b = luaL_optinteger(L,  9, 255);
		v.a = luaL_optinteger(L, 10, 255);
	}

	EXCEPT_GUARD(t->setVertex(i, v);)
	return 0;
}

int w_Mesh_getVertex(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	size_t i = (size_t) (luaL_checkinteger(L, 2) - 1);

	Vertex v;
	EXCEPT_GUARD(v = t->getVertex(i);)

	lua_pushnumber(L, v.x);
	lua_pushnumber(L, v.y);
	lua_pushnumber(L, v.s);
	lua_pushnumber(L, v.t);
	lua_pushnumber(L, v.r);
	lua_pushnumber(L, v.g);
	lua_pushnumber(L, v.b);
	lua_pushnumber(L, v.a);

	return 8;
}

int w_Mesh_setVertices(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);

	size_t vertex_count = lua_objlen(L, 2);
	std::vector<Vertex> vertices;
	vertices.reserve(vertex_count);

	// Get the vertices from the table.
	for (size_t i = 1; i <= vertex_count; i++)
	{
		lua_rawgeti(L, 2, i);

		if (lua_type(L, -1) != LUA_TTABLE)
			return luax_typerror(L, 2, "table of tables");

		for (int j = 1; j <= 8; j++)
			lua_rawgeti(L, -j, j);

		Vertex v;

		v.x = (float) luaL_checknumber(L, -8);
		v.y = (float) luaL_checknumber(L, -7);

		v.s = (float) luaL_checknumber(L, -6);
		v.t = (float) luaL_checknumber(L, -5);

		v.r = (unsigned char) luaL_optinteger(L, -4, 255);
		v.g = (unsigned char) luaL_optinteger(L, -3, 255);
		v.b = (unsigned char) luaL_optinteger(L, -2, 255);
		v.a = (unsigned char) luaL_optinteger(L, -1, 255);

		lua_pop(L, 9);
		vertices.push_back(v);
	}

	EXCEPT_GUARD(t->setVertices(vertices);)
	return 0;
}

int w_Mesh_getVertexCount(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	lua_pushinteger(L, t->getVertexCount());
	return 1;
}

int w_Mesh_setVertexMap(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);

	bool is_table = lua_istable(L, 2);
	int nargs = is_table ? lua_objlen(L, 2) : lua_gettop(L) - 1;

	std::vector<uint32> vertexmap;
	vertexmap.reserve(nargs);

	for (int i = 0; i < nargs; i++)
	{
		if (is_table)
		{
			lua_rawgeti(L, 2, i + 1);
			vertexmap.push_back(uint32(luaL_checkinteger(L, -1) - 1));
			lua_pop(L, 1);
		}
		else
			vertexmap.push_back(uint32(luaL_checkinteger(L, i + 2) - 1));
	}

	EXCEPT_GUARD(t->setVertexMap(vertexmap);)
	return 0;
}

int w_Mesh_getVertexMap(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	const uint32 *vertex_map = 0;

	EXCEPT_GUARD(vertex_map = t->getVertexMap();)
	size_t elements = t->getVertexMapCount();

	lua_createtable(L, elements, 0);

	for (size_t i = 0; i < elements; i++)
	{
		lua_pushinteger(L, lua_Integer(vertex_map[i]) + 1);
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

int w_Mesh_setImage(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);

	if (lua_isnoneornil(L, 2))
		t->setImage();
	else
	{
		Image *img = luax_checkimage(L, 2);
		t->setImage(img);
	}

	return 0;
}

int w_Mesh_getImage(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	Image *img = t->getImage();

	if (img == NULL)
		return 0;

	img->retain();
	luax_pushtype(L, "Image", GRAPHICS_IMAGE_T, img);
	return 1;
}

int w_Mesh_setDrawMode(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	const char *str = luaL_checkstring(L, 2);
	Mesh::DrawMode mode;

	if (!Mesh::getConstant(str, mode))
		return luaL_error(L, "Invalid mesh draw mode: %s", str);

	t->setDrawMode(mode);
	return 0;
}

int w_Mesh_getDrawMode(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	Mesh::DrawMode mode = t->getDrawMode();
	const char *str;

	if (!Mesh::getConstant(mode, str))
		return luaL_error(L, "Unknown mesh draw mode.");

	lua_pushstring(L, str);
	return 1;
}

int w_Mesh_setVertexColors(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	t->setVertexColors(luax_toboolean(L, 2));
	return 0;
}

int w_Mesh_hasVertexColors(lua_State *L)
{
	Mesh *t = luax_checkmesh(L, 1);
	luax_pushboolean(L, t->hasVertexColors());
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "setVertex", w_Mesh_setVertex },
	{ "getVertex", w_Mesh_getVertex },
	{ "setVertices", w_Mesh_setVertices },
	{ "getVertexCount", w_Mesh_getVertexCount },
	{ "setVertexMap", w_Mesh_setVertexMap },
	{ "getVertexMap", w_Mesh_getVertexMap },
	{ "setImage", w_Mesh_setImage },
	{ "getImage", w_Mesh_getImage },
	{ "setDrawMode", w_Mesh_setDrawMode },
	{ "getDrawMode", w_Mesh_getDrawMode },
	{ "setVertexColors", w_Mesh_setVertexColors },
	{ "hasVertexColors", w_Mesh_hasVertexColors },
	{ 0, 0 }
};

extern "C" int luaopen_mesh(lua_State *L)
{
	return luax_register_type(L, "Mesh", functions);
}

} // opengl
} // graphics
} // love
