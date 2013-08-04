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

#include "wrap_RandomGenerator.h"

#include <cmath>
#include <algorithm>

namespace love
{
namespace math
{

template <typename T>
static T checkrandomstate_part(lua_State *L, int idx)
{
	double num = luaL_checknumber(L, idx);
	double inf = std::numeric_limits<double>::infinity();

	// Disallow conversions from infinity and NaN.
	if (num == inf || num == -inf || num != num)
		luaL_argerror(L, idx, "invalid random state");

	return (T) num;
}

RandomGenerator::State luax_checkrandomstate(lua_State *L, int idx)
{
	RandomGenerator::State s;

	if (!lua_isnoneornil(L, idx + 1))
	{
		uint32 low = checkrandomstate_part<uint32>(L, idx);
		uint32 high = checkrandomstate_part<uint32>(L, idx + 1);

#ifdef LOVE_BIG_ENDIAN
		s.b32.a = high;
		s.b32.b = low;
#else
		s.b32.b = high;
		s.b32.a = low;
#endif
	}
	else
		s.b64 = checkrandomstate_part<uint64>(L, idx);

	return s;
}

int luax_getrandom(lua_State *L, int startidx, double r)
{
	int l, u;
	// from lua 5.1.4 source code: lmathlib.c:185 ff.
	switch (lua_gettop(L) - (startidx - 1))
	{
	case 0:
		lua_pushnumber(L, r);
		break;
	case 1:
		u = luaL_checkint(L, startidx);
		luaL_argcheck(L, 1 <= u, startidx, "interval is empty");
		lua_pushnumber(L, floor(r * u) + 1);
		break;
	case 2:
		l = luaL_checkint(L, startidx);
		u = luaL_checkint(L, startidx + 1);
		luaL_argcheck(L, l <= u, startidx + 1, "interval is empty");
		lua_pushnumber(L, floor(r * (u - l + 1)) + l);
		break;
	default:
		return luaL_error(L, "wrong number of arguments");
	}
	return 1;
}

RandomGenerator *luax_checkrandomgenerator(lua_State *L, int idx)
{
	return luax_checktype<RandomGenerator>(L, idx, "RandomGenerator", MATH_RANDOM_GENERATOR_T);
}

int w_RandomGenerator_setState(lua_State *L)
{
	RandomGenerator *rng = luax_checkrandomgenerator(L, 1);
	try
	{
		rng->setState(luax_checkrandomstate(L, 2));
	}
	catch (love::Exception &e)
	{
		return luaL_error(L, "%s", e.what());
	}
	return 0;
}

int w_RandomGenerator_getState(lua_State *L)
{
	RandomGenerator *rng = luax_checkrandomgenerator(L, 1);

	uint32 low = 0, high = 0;
	rng->getState(low, high);

	lua_pushnumber(L, (lua_Number) low);
	lua_pushnumber(L, (lua_Number) high);
	return 2;
}

int w_RandomGenerator_random(lua_State *L)
{
	RandomGenerator *rng = luax_checkrandomgenerator(L, 1);
	return luax_getrandom(L, 2, rng->random());
}

int w_RandomGenerator_randomNormal(lua_State *L)
{
	RandomGenerator *rng = luax_checkrandomgenerator(L, 1);

	double stddev = luaL_optnumber(L, 2, 1.0);
	double mean = luaL_optnumber(L, 3, 0.0);
	double r = rng->randomNormal(stddev);

	lua_pushnumber(L, r + mean);
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "setState", w_RandomGenerator_setState },
	{ "getState", w_RandomGenerator_getState },
	{ "random", w_RandomGenerator_random },
	{ "randomNormal", w_RandomGenerator_randomNormal },
	{ 0, 0 }
};

extern "C" int luaopen_randomgenerator(lua_State *L)
{
	return luax_register_type(L, "RandomGenerator", functions);
}

} // math
} // love
