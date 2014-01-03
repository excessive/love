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

#include "wrap_CircleShape.h"

namespace love
{
namespace physics
{
namespace box2d
{

CircleShape *luax_checkcircleshape(lua_State *L, int idx)
{
	return luax_checktype<CircleShape>(L, idx, "CircleShape", PHYSICS_CIRCLE_SHAPE_T);
}

int w_CircleShape_getRadius(lua_State *L)
{
	CircleShape *c = luax_checkcircleshape(L, 1);
	lua_pushnumber(L, c->getRadius());
	return 1;
}

int w_CircleShape_setRadius(lua_State *L)
{
	CircleShape *c = luax_checkcircleshape(L, 1);
	float r = (float)luaL_checknumber(L, 2);
	c->setRadius(r);
	return 0;
}

int w_CircleShape_getPoint(lua_State *L)
{
	CircleShape *c = luax_checkcircleshape(L, 1);
	float x, y;
	c->getPoint(x, y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

int w_CircleShape_setPoint(lua_State *L)
{
	CircleShape *c = luax_checkcircleshape(L, 1);
	c->setPoint((float) luaL_checknumber(L, 2), (float) luaL_checknumber(L, 3));
	return 0;
}

static const luaL_Reg functions[] =
{
	{ "getRadius", w_CircleShape_getRadius },
	{ "setRadius", w_CircleShape_setRadius },
	{ "getPoint", w_CircleShape_getPoint },
	{ "setPoint", w_CircleShape_setPoint },
	// From Shape.
	{ "getType", w_Shape_getType },
	{ "getRadius", w_Shape_getRadius },
	{ "getChildCount", w_Shape_getChildCount },
	{ "testPoint", w_Shape_testPoint },
	{ "rayCast", w_Shape_rayCast },
	{ "computeAABB", w_Shape_computeAABB },
	{ "computeMass", w_Shape_computeMass },
	{ 0, 0 }
};

extern "C" int luaopen_circleshape(lua_State *L)
{
	return luax_register_type(L, "CircleShape", functions);
}

} // box2d
} // physics
} // love
