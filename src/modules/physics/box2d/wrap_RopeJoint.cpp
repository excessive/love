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

#include "wrap_RopeJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{

RopeJoint *luax_checkropejoint(lua_State *L, int idx)
{
	RopeJoint *j = luax_checktype<RopeJoint>(L, idx);
	if (!j->isValid())
		luaL_error(L, "Attempt to use destroyed joint.");
	return j;
}

int w_RopeJoint_getMaxLength(lua_State *L)
{
	RopeJoint *t = luax_checkropejoint(L, 1);
	lua_pushnumber(L, t->getMaxLength());
	return 1;
}

int w_RopeJoint_setMaxLength(lua_State *L)
{
	RopeJoint *t = luax_checkropejoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	t->setMaxLength(arg1);
	return 0;
}
	
static const luaL_Reg w_RopeJoint_functions[] =
{
	{ "getMaxLength", w_RopeJoint_getMaxLength },
	{ "setMaxLength", w_RopeJoint_setMaxLength },
	{ 0, 0 }
};

extern "C" int luaopen_ropejoint(lua_State *L)
{
	return luax_register_type(L, &RopeJoint::type, w_Joint_functions, w_RopeJoint_functions, nullptr);
}

} // box2d
} // physics
} // love
