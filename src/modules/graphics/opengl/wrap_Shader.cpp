/**
 * Copyright (c) 2006-2016 LOVE Development Team
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

#include "wrap_Shader.h"
#include "graphics/wrap_Texture.h"
#include "math/MathModule.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace love
{
namespace graphics
{
namespace opengl
{

Shader *luax_checkshader(lua_State *L, int idx)
{
	return luax_checktype<Shader>(L, idx, GRAPHICS_SHADER_ID);
}

int w_Shader_getWarnings(lua_State *L)
{
	Shader *shader = luax_checkshader(L, 1);
	lua_pushstring(L, shader->getWarnings().c_str());
	return 1;
}

template <typename T>
static T *_getScalars(lua_State *L, Shader *shader, int count, size_t &dimension)
{
	dimension = 1;
	T *values = shader->getScratchBuffer<T>(count);

	for (int i = 0; i < count; ++i)
	{
		if (lua_isnumber(L, 3 + i))
			values[i] = static_cast<T>(lua_tonumber(L, 3 + i));
		else if (lua_isboolean(L, 3 + i))
			values[i] = static_cast<T>(lua_toboolean(L, 3 + i));
		else
		{
			luax_typerror(L, 3 + i, "number or boolean");
			return 0;
		}
	}

	return values;
}

template <typename T>
static T *_getVectors(lua_State *L, Shader *shader, int count, size_t &dimension)
{
	dimension = luax_objlen(L, 3);
	T *values = shader->getScratchBuffer<T>(count * dimension);

	for (int i = 0; i < count; ++i)
	{
		if (!lua_istable(L, 3 + i))
		{
			luax_typerror(L, 3 + i, "table");
			return 0;
		}
		if (luax_objlen(L, 3 + i) != dimension)
		{
			luaL_error(L, "Error in argument %d: Expected table size %d, got %d.",
			           3+i, dimension, luax_objlen(L, 3+i));
			return 0;
		}

		for (int k = 1; k <= (int) dimension; ++k)
		{
			lua_rawgeti(L, 3 + i, k);
			if (lua_isnumber(L, -1))
				values[i * dimension + k - 1] = static_cast<T>(lua_tonumber(L, -1));
			else if (lua_isboolean(L, -1))
				values[i * dimension + k - 1] = static_cast<T>(lua_toboolean(L, -1));
			else
			{
				luax_typerror(L, -1, "number or boolean");
				return 0;
			}
		}
		lua_pop(L, int(dimension));
	}

	return values;
}

int w_Shader_sendInt(lua_State *L)
{
	Shader *shader = luax_checkshader(L, 1);
	const char *name = luaL_checkstring(L, 2);
	int count = lua_gettop(L) - 2;

	if (count < 1)
		return luaL_error(L, "No variable to send.");

	int *values = 0;
	size_t dimension = 1;

	if (lua_isnumber(L, 3) || lua_isboolean(L, 3))
		values = _getScalars<int>(L, shader, count, dimension);
	else if (lua_istable(L, 3))
		values = _getVectors<int>(L, shader, count, dimension);
	else
		return luax_typerror(L, 3, "number, boolean, or table");

	if (!values)
		return luaL_error(L, "Error in arguments.");

	luax_catchexcept(L, [&]() { shader->sendInt(name, (int) dimension, values, count); });
	return 0;
}

static int w__Shader_sendFloat(lua_State *L, bool colors)
{
	Shader *shader = luax_checkshader(L, 1);
	const char *name = luaL_checkstring(L, 2);
	int count = lua_gettop(L) - 2;

	if (count < 1)
		return luaL_error(L, "No variable to send.");

	float *values = nullptr;
	size_t dimension = 1;

	if (lua_isnumber(L, 3) || lua_isboolean(L, 3))
		values = _getScalars<float>(L, shader, count, dimension);
	else if (lua_istable(L, 3))
		values = _getVectors<float>(L, shader, count, dimension);
	else
		return luax_typerror(L, 3, "number, boolean, or table");

	if (!values)
		return luaL_error(L, "Error in arguments.");

	if (colors && love::graphics::isGammaCorrect())
	{
		// the fourth component (alpha) is always already linear, if it exists.
		int ncomponents = std::min((int) dimension, 3);

		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < ncomponents; j++)
				values[i * dimension + j] = math::gammaToLinear(values[i * dimension + j]);
		}
	}

	luax_catchexcept(L, [&]() { shader->sendFloat(name, (int) dimension, values, count); });
	return 0;
}

int w_Shader_sendFloat(lua_State *L)
{
	return w__Shader_sendFloat(L, false);
}

int w_Shader_sendColor(lua_State *L)
{
	return w__Shader_sendFloat(L, true);
}

int w_Shader_sendMatrix(lua_State *L)
{
	int count = lua_gettop(L) - 2;
	Shader *shader = luax_checkshader(L, 1);
	const char *name = luaL_checkstring(L, 2);

	if (!lua_istable(L, 3))
		return luax_typerror(L, 3, "matrix table");

	int dimension = 0;

	lua_rawgeti(L, 3, 1);
	if (lua_istable(L, -1))
		dimension = (int) luax_objlen(L, 3);
	lua_pop(L, 1);

	if (dimension == 0)
	{
		lua_getfield(L, 3, "dimension");

		if (!lua_isnoneornil(L, -1))
			dimension = (int) lua_tointeger(L, -1);
		else
			dimension = (int) sqrtf((float) luax_objlen(L, 3));

		lua_pop(L, 1);
	}

	if (dimension < 2 || dimension > 4)
		return luaL_error(L, "Invalid matrix size: %dx%d (only 2x2, 3x3 and 4x4 matrices are supported).",
						  dimension, dimension);

	float *values = shader->getScratchBuffer<float>(dimension * dimension * count);

	for (int i = 0; i < count; ++i)
	{
		int other_dimension = 0;

		lua_rawgeti(L, 3+i, 1);
		bool table_of_tables = lua_istable(L, -1);

		if (table_of_tables)
			other_dimension = luax_objlen(L, -1);

		lua_pop(L, 1);

		if (!table_of_tables)
			other_dimension = (int) sqrtf((float) luax_objlen(L, 3+i));

		if (other_dimension != dimension)
		{
			// You unlock this door with the key of imagination. Beyond it is
			// another dimension: a dimension of sound, a dimension of sight,
			// a dimension of mind. You're moving into a land of both shadow
			// and substance, of things and ideas. You've just crossed over
			// into... the Twilight Zone.
			return luaL_error(L, "Invalid matrix size at argument %d: Expected size %dx%d, got %dx%d.",
			                  3+i, dimension, dimension, other_dimension, other_dimension);
		}

		if (table_of_tables)
		{
			int n = 0;

			for (int j = 1; j <= dimension; j++)
			{
				lua_rawgeti(L, 3+i, j);

				for (int k = 1; k <= dimension; k++)
				{
					lua_rawgeti(L, -k, k);
					values[i * dimension * dimension + n] = (float) lua_tonumber(L, -1);
					n++;
				}

				lua_pop(L, dimension + 1);
			}
		}
		else
		{
			for (int k = 1; k <= dimension*dimension; k++)
			{
				lua_rawgeti(L, 3+i, k);
				values[i * dimension * dimension + k - 1] = (float) lua_tonumber(L, -1);
			}

			lua_pop(L, dimension*dimension);
		}
	}

	luax_catchexcept(L, [&]() { shader->sendMatrix(name, dimension, values, count); });
	return 0;
}

int w_Shader_sendTexture(lua_State *L)
{
	Shader *shader = luax_checkshader(L, 1);
	const char *name = luaL_checkstring(L, 2);
	Texture *texture = luax_checktexture(L, 3);

	luax_catchexcept(L, [&](){ shader->sendTexture(name, texture); });
	return 0;
}

int w_Shader_send(lua_State *L)
{
	int ttype = lua_type(L, 3);
	Proxy *p = nullptr;

	switch (ttype)
	{
	case LUA_TNUMBER:
	case LUA_TBOOLEAN:
		// Scalar float/boolean.
		return w_Shader_sendFloat(L);
		break;
	case LUA_TUSERDATA:
		// Texture (Image or Canvas).
		p = (Proxy *) lua_touserdata(L, 3);

		if (typeFlags[p->type][GRAPHICS_TEXTURE_ID])
			return w_Shader_sendTexture(L);

		break;
	case LUA_TTABLE:
		// Vector or Matrix.
		lua_rawgeti(L, 3, 1);
		ttype = lua_type(L, -1);
		lua_pop(L, 1);

		if (ttype == LUA_TNUMBER || ttype == LUA_TBOOLEAN)
			return w_Shader_sendFloat(L);
		else if (ttype == LUA_TTABLE)
			return w_Shader_sendMatrix(L);

		break;
	default:
		break;
	}

	return luaL_argerror(L, 3, "number, boolean, table, image, or canvas expected");
}

int w_Shader_hasUniform(lua_State *L)
{
	Shader *shader = luax_checkshader(L, 1);
	const char *name = luaL_checkstring(L, 2);
	luax_pushboolean(L, shader->hasUniform(name));
	return 1;
}

static const luaL_Reg w_Shader_functions[] =
{
	{ "getWarnings", w_Shader_getWarnings },
	{ "sendInt",     w_Shader_sendInt },
	{ "sendBoolean", w_Shader_sendInt },
	{ "sendFloat",   w_Shader_sendFloat },
	{ "sendColor",   w_Shader_sendColor },
	{ "sendMatrix",  w_Shader_sendMatrix },
	{ "sendTexture", w_Shader_sendTexture },
	{ "send",        w_Shader_send },
	{ "hasUniform",  w_Shader_hasUniform },
	{ 0, 0 }
};

extern "C" int luaopen_shader(lua_State *L)
{
	return luax_register_type(L, GRAPHICS_SHADER_ID, "Shader", w_Shader_functions, nullptr);
}

} // opengl
} // graphics
} // love

