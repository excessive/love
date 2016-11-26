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

#include "wrap_Math.h"
#include "wrap_RandomGenerator.h"
#include "wrap_BezierCurve.h"
#include "wrap_CompressedData.h"
#include "wrap_Transform.h"
#include "MathModule.h"
#include "BezierCurve.h"
#include "Transform.h"
#include "common/b64.h"

#include <cmath>
#include <iostream>
#include <algorithm>

// Put the Lua code directly into a raw string literal.
static const char math_lua[] =
#include "wrap_Math.lua"
;

namespace love
{
namespace math
{

int w__getRandomGenerator(lua_State *L)
{
	RandomGenerator *t = Math::instance.getRandomGenerator();
	luax_pushtype(L, t);
	return 1;
}

int w_newRandomGenerator(lua_State *L)
{
	RandomGenerator::Seed s;
	if (lua_gettop(L) > 0)
		s = luax_checkrandomseed(L, 1);

	RandomGenerator *t = Math::instance.newRandomGenerator();

	if (lua_gettop(L) > 0)
	{
		bool should_error = false;

		try
		{
			t->setSeed(s);
		}
		catch (love::Exception &e)
		{
			t->release();
			should_error = true;
			lua_pushstring(L, e.what());
		}

		if (should_error)
			return luaL_error(L, "%s", lua_tostring(L, -1));
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_newBezierCurve(lua_State *L)
{
	std::vector<Vector> points;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		points.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			Vector v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			points.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = (int) lua_gettop(L);
		points.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			Vector v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			points.push_back(v);
		}
	}

	BezierCurve *curve = Math::instance.newBezierCurve(points);
	luax_pushtype(L, curve);
	curve->release();
	return 1;
}

int w_newTransform(lua_State *L)
{
	Transform *t = nullptr;

	if (lua_isnoneornil(L, 1))
		t = Math::instance.newTransform();
	else
	{
		float x =  (float) luaL_checknumber(L, 1);
		float y =  (float) luaL_checknumber(L, 2);
		float a =  (float) luaL_optnumber(L, 3, 0.0);
		float sx = (float) luaL_optnumber(L, 4, 1.0);
		float sy = (float) luaL_optnumber(L, 5, sx);
		float ox = (float) luaL_optnumber(L, 6, 0.0);
		float oy = (float) luaL_optnumber(L, 7, 0.0);
		float kx = (float) luaL_optnumber(L, 8, 0.0);
		float ky = (float) luaL_optnumber(L, 9, 0.0);
		t = Math::instance.newTransform(x, y, a, sx, sy, ox, oy, kx, ky);
	}

	luax_pushtype(L, t);
	t->release();
	return 1;
}

int w_triangulate(lua_State *L)
{
	std::vector<love::Vector> vertices;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			Vector v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			vertices.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = (int) lua_gettop(L);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			Vector v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			vertices.push_back(v);
		}
	}

	if (vertices.size() < 3)
		return luaL_error(L, "Need at least 3 vertices to triangulate");

	std::vector<Triangle> triangles;

	luax_catchexcept(L, [&]() {
		if (vertices.size() == 3)
			triangles.push_back(Triangle(vertices[0], vertices[1], vertices[2]));
		else
			triangles = triangulate(vertices);
	});

	lua_createtable(L, (int) triangles.size(), 0);
	for (int i = 0; i < (int) triangles.size(); ++i)
	{
		const Triangle &tri = triangles[i];

		lua_createtable(L, 6, 0);
		lua_pushnumber(L, tri.a.x);
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, tri.a.y);
		lua_rawseti(L, -2, 2);
		lua_pushnumber(L, tri.b.x);
		lua_rawseti(L, -2, 3);
		lua_pushnumber(L, tri.b.y);
		lua_rawseti(L, -2, 4);
		lua_pushnumber(L, tri.c.x);
		lua_rawseti(L, -2, 5);
		lua_pushnumber(L, tri.c.y);
		lua_rawseti(L, -2, 6);

		lua_rawseti(L, -2, i+1);
	}

	return 1;
}

int w_isConvex(lua_State *L)
{
	std::vector<love::Vector> vertices;
	if (lua_istable(L, 1))
	{
		int top = (int) luax_objlen(L, 1);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			lua_rawgeti(L, 1, i);
			lua_rawgeti(L, 1, i+1);

			love::Vector v;
			v.x = (float) luaL_checknumber(L, -2);
			v.y = (float) luaL_checknumber(L, -1);
			vertices.push_back(v);

			lua_pop(L, 2);
		}
	}
	else
	{
		int top = lua_gettop(L);
		vertices.reserve(top / 2);
		for (int i = 1; i <= top; i += 2)
		{
			love::Vector v;
			v.x = (float) luaL_checknumber(L, i);
			v.y = (float) luaL_checknumber(L, i+1);
			vertices.push_back(v);
		}
	}

	luax_pushboolean(L, isConvex(vertices));
	return 1;
}

static int getGammaArgs(lua_State *L, float color[4])
{
	int numcomponents = 0;

	if (lua_istable(L, 1))
	{
		int n = (int) luax_objlen(L, 1);
		for (int i = 1; i <= n && i <= 4; i++)
		{
			lua_rawgeti(L, 1, i);
			color[i - 1] = (float) luaL_checknumber(L, -1);
			numcomponents++;
		}

		lua_pop(L, numcomponents);
	}
	else
	{
		int n = lua_gettop(L);
		for (int i = 1; i <= n && i <= 4; i++)
		{
			color[i - 1] = (float) luaL_checknumber(L, i);
			numcomponents++;
		}
	}

	if (numcomponents == 0)
		luaL_checknumber(L, 1);

	return numcomponents;
}

int w_gammaToLinear(lua_State *L)
{
	float color[4];
	int numcomponents = getGammaArgs(L, color);

	for (int i = 0; i < numcomponents; i++)
	{
		// Alpha should always be linear.
		if (i < 3)
			color[i] = gammaToLinear(color[i]);
		lua_pushnumber(L, color[i]);
	}

	return numcomponents;
}

int w_linearToGamma(lua_State *L)
{
	float color[4];
	int numcomponents = getGammaArgs(L, color);

	for (int i = 0; i < numcomponents; i++)
	{
		// Alpha should always be linear.
		if (i < 3)
			color[i] = linearToGamma(color[i]);
		lua_pushnumber(L, color[i]);
	}

	return numcomponents;
}

int w_noise(lua_State *L)
{
	int nargs = std::min(std::max(lua_gettop(L), 1), 4);
	float args[4];

	for (int i = 0; i < nargs; i++)
		args[i] = (float) luaL_checknumber(L, i + 1);

	float val = 0.0f;

	switch (nargs)
	{
	case 1:
		val = noise1(args[0]);
		break;
	case 2:
		val = noise2(args[0], args[1]);
		break;
	case 3:
		val = noise3(args[0], args[1], args[2]);
		break;
	case 4:
		val = noise4(args[0], args[1], args[2], args[3]);
		break;
	}

	lua_pushnumber(L, (lua_Number) val);
	return 1;
}

int w_compress(lua_State *L)
{
	const char *fstr = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);
	Compressor::Format format = Compressor::FORMAT_LZ4;

	if (fstr && !Compressor::getConstant(fstr, format))
		return luaL_error(L, "Invalid compressed data format: %s", fstr);

	int level = (int) luaL_optnumber(L, 3, -1);

	CompressedData *cdata = nullptr;
	if (lua_isstring(L, 1))
	{
		size_t rawsize = 0;
		const char *rawbytes = luaL_checklstring(L, 1, &rawsize);
		luax_catchexcept(L, [&](){ cdata = compress(format, rawbytes, rawsize, level); });
	}
	else
	{
		Data *rawdata = luax_checktype<Data>(L, 1);
		luax_catchexcept(L, [&](){ cdata = compress(format, rawdata, level); });
	}

	luax_pushtype(L, cdata);
	return 1;
}

int w_decompress(lua_State *L)
{
	char *rawbytes = nullptr;
	size_t rawsize = 0;

	if (luax_istype(L, 1, CompressedData::type))
	{
		CompressedData *data = luax_checkcompresseddata(L, 1);
		rawsize = data->getDecompressedSize();
		luax_catchexcept(L, [&](){ rawbytes = decompress(data, rawsize); });
	}
	else
	{
		Compressor::Format format = Compressor::FORMAT_LZ4;
		const char *fstr = luaL_checkstring(L, 2);

		if (!Compressor::getConstant(fstr, format))
			return luaL_error(L, "Invalid compressed data format: %s", fstr);

		size_t compressedsize = 0;
		const char *cbytes = nullptr;

		if (luax_istype(L, 1, Data::type))
		{
			Data *data = luax_checktype<Data>(L, 1);
			cbytes = (const char *) data->getData();
			compressedsize = data->getSize();
		}
		else
			cbytes = luaL_checklstring(L, 1, &compressedsize);

		luax_catchexcept(L, [&](){ rawbytes = decompress(format, cbytes, compressedsize, rawsize); });
	}

	lua_pushlstring(L, rawbytes, rawsize);
	delete[] rawbytes;

	return 1;
}

int w_encode(lua_State *L)
{
	const char *formatstr = luaL_checkstring(L, 1);
	EncodeFormat format;
	if (!getConstant(formatstr, format))
		return luaL_error(L, "Invalid encode format: %s", formatstr);

	size_t srclen = 0;
	const char *src = nullptr;

	if (luax_istype(L, 2, Data::type))
	{
		Data *data = luax_totype<Data>(L, 2);
		src = (const char *) data->getData();
		srclen = data->getSize();
	}
	else
		src = luaL_checklstring(L, 2, &srclen);

	size_t linelen = (size_t) luaL_optinteger(L, 3, 0);

	size_t dstlen = 0;
	char *dst = nullptr;
	luax_catchexcept(L, [&](){ dst = encode(format, src, srclen, dstlen, linelen); });

	if (dst != nullptr)
		lua_pushlstring(L, dst, dstlen);
	else
		lua_pushstring(L, "");

	delete[] dst;
	return 1;
}

int w_decode(lua_State *L)
{
	const char *formatstr = luaL_checkstring(L, 1);
	EncodeFormat format;
	if (!getConstant(formatstr, format))
		return luaL_error(L, "Invalid decode format: %s", formatstr);

	size_t srclen = 0;
	const char *src = nullptr;

	if (luax_istype(L, 2, Data::type))
	{
		Data *data = luax_totype<Data>(L, 2);
		src = (const char *) data->getData();
		srclen = data->getSize();
	}
	else
		src = luaL_checklstring(L, 2, &srclen);

	size_t dstlen = 0;
	char *dst = nullptr;
	luax_catchexcept(L, [&](){ dst = decode(format, src, srclen, dstlen); });

	if (dst != nullptr)
		lua_pushlstring(L, dst, dstlen);
	else
		lua_pushstring(L, "");

	delete[] dst;
	return 1;
}

int w_hash(lua_State *L)
{
	const char *fstr = luaL_checkstring(L, 1);
	HashFunction::Function function;
	if (!HashFunction::getConstant(fstr, function))
		return luaL_error(L, "Invalid hash function: %s", fstr);

	std::string hash;
	if (lua_isstring(L, 2))
	{
		size_t rawsize = 0;
		const char *rawbytes = luaL_checklstring(L, 2, &rawsize);
		luax_catchexcept(L, [&](){ hash = love::math::hash(function, rawbytes, rawsize); });
	}
	else
	{
		Data *rawdata = luax_checktype<Data>(L, 2);
		luax_catchexcept(L, [&](){ hash = love::math::hash(function, rawdata); });
	}

	luax_pushstring(L, hash);
	return 1;
}

// C functions in a struct, necessary for the FFI versions of math functions.
struct FFI_Math
{
	float (*noise1)(float x);
	float (*noise2)(float x, float y);
	float (*noise3)(float x, float y, float z);
	float (*noise4)(float x, float y, float z, float w);

	float (*gammaToLinear)(float c);
	float (*linearToGamma)(float c);
};

static FFI_Math ffifuncs =
{
	noise1,
	noise2,
	noise3,
	noise4,

	gammaToLinear,
	linearToGamma,
};

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	// love.math.random etc. are defined in wrap_Math.lua.

	{ "_getRandomGenerator", w__getRandomGenerator },
	{ "newRandomGenerator", w_newRandomGenerator },
	{ "newBezierCurve", w_newBezierCurve },
	{ "newTransform", w_newTransform },
	{ "triangulate", w_triangulate },
	{ "isConvex", w_isConvex },
	{ "gammaToLinear", w_gammaToLinear },
	{ "linearToGamma", w_linearToGamma },
	{ "noise", w_noise },
	{ "compress", w_compress },
	{ "decompress", w_decompress },
	{ "encode", w_encode },
	{ "decode", w_decode },
	{ "hash", w_hash },
	{ 0, 0 }
};

static const lua_CFunction types[] =
{
	luaopen_randomgenerator,
	luaopen_beziercurve,
	luaopen_compresseddata,
	luaopen_transform,
	0
};

extern "C" int luaopen_love_math(lua_State *L)
{
	Math::instance.retain();

	WrappedModule w;
	w.module = &Math::instance;
	w.name = "math";
	w.type = &Module::type;
	w.functions = functions;
	w.types = types;

	int n = luax_register_module(L, w);

	// Execute wrap_Math.lua, sending the math table and ffifuncs pointer as args.
	luaL_loadbuffer(L, math_lua, sizeof(math_lua), "wrap_Math.lua");
	lua_pushvalue(L, -2);
	lua_pushlightuserdata(L, &ffifuncs);
	lua_call(L, 2, 0);

	return n;
}

} // math
} // love
