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

#include "common/Exception.h"
#include "wrap_BezierCurve.h"

#include <cmath>

namespace love
{
namespace math
{

BezierCurve *luax_checkbeziercurve(lua_State *L, int idx)
{
	return luax_checktype<BezierCurve>(L, idx, "BezierCurve", MATH_BEZIER_CURVE_T);
}

int w_BezierCurve_getDegree(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	lua_pushnumber(L, curve->getDegree());
	return 1;
}

int w_BezierCurve_getControlPoint(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	int idx = luaL_checkinteger(L, 2);

	if (idx > 0) // 1-indexing
		idx--;

	try
	{
		Vector v = curve->getControlPoint(idx);
		lua_pushnumber(L, v.x);
		lua_pushnumber(L, v.y);
	}
	catch (Exception &e)
	{
		return luaL_error(L, e.what());
	}

	return 2;
}

int w_BezierCurve_setControlPoint(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	int idx = luaL_checkinteger(L, 2);
	double vx = luaL_checknumber(L, 3);
	double vy = luaL_checknumber(L, 4);

	if (idx > 0) // 1-indexing
		idx--;

	try
	{
		curve->setControlPoint(idx, Vector(vx,vy));
	}
	catch (Exception &e)
	{
		return luaL_error(L, e.what());
	}

	return 0;
}

int w_BezierCurve_insertControlPoint(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	double vx = luaL_checknumber(L, 2);
	double vy = luaL_checknumber(L, 3);
	int idx = luaL_optinteger(L, 4, -1);

	if (idx > 0) // 1-indexing
		idx--;

	try
	{
		curve->insertControlPoint(Vector(vx,vy), idx);
	}
	catch (Exception &e)
	{
		return luaL_error(L, e.what());
	}

	return 0;
}

int w_BezierCurve_eval(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	double t = luaL_checknumber(L, 2);

	try
	{
		Vector v = curve->eval(t);
		lua_pushnumber(L, v.x);
		lua_pushnumber(L, v.y);
	}
	catch (Exception &e)
	{
		return luaL_error(L, e.what());
	}

	return 2;

}

int w_BezierCurve_render(lua_State *L)
{
	BezierCurve *curve = luax_checkbeziercurve(L, 1);
	int accuracy = luaL_optinteger(L, 2, 5);

	std::vector<Vector> points = curve->render(accuracy);

	lua_createtable(L, points.size()*2, 0);
	for (size_t i = 0; i < points.size(); ++i)
	{
		lua_pushnumber(L, points[i].x);
		lua_rawseti(L, -2, 2*i+1);
		lua_pushnumber(L, points[i].y);
		lua_rawseti(L, -2, 2*i+2);
	}

	return 1;
}

static const luaL_Reg functions[] =
{
	{"getDegree", w_BezierCurve_getDegree},
	{"getControlPoint", w_BezierCurve_getControlPoint},
	{"setControlPoint", w_BezierCurve_setControlPoint},
	{"insertControlPoint", w_BezierCurve_insertControlPoint},
	{"eval", w_BezierCurve_eval},
	{"render", w_BezierCurve_render},
	{ 0, 0 }
};

extern "C" int luaopen_beziercurve(lua_State *L)
{
	return luax_register_type(L, "BezierCurve", functions);
}

} // math
} // love
