/**
 * Copyright (c) 2006-2012 LOVE Development Team
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

#include "wrap_FrictionJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{

FrictionJoint *luax_checkfrictionjoint(lua_State *L, int idx)
{
	FrictionJoint *j = luax_checktype<FrictionJoint>(L, idx, "FrictionJoint", PHYSICS_FRICTION_JOINT_T);
	if (!j->isValid())
		luaL_error(L, "Attempt to use destroyed joint.");
	return j;
}

int w_FrictionJoint_setMaxForce(lua_State *L)
{
	FrictionJoint *t = luax_checkfrictionjoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	t->setMaxForce(arg1);
	return 0;
}

int w_FrictionJoint_getMaxForce(lua_State *L)
{
	FrictionJoint *t = luax_checkfrictionjoint(L, 1);
	lua_pushnumber(L, t->getMaxForce());
	return 1;
}

int w_FrictionJoint_setMaxTorque(lua_State *L)
{
	FrictionJoint *t = luax_checkfrictionjoint(L, 1);
	float arg1 = (float)luaL_checknumber(L, 2);
	t->setMaxTorque(arg1);
	return 0;
}

int w_FrictionJoint_getMaxTorque(lua_State *L)
{
	FrictionJoint *t = luax_checkfrictionjoint(L, 1);
	lua_pushnumber(L, t->getMaxTorque());
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "setMaxForce", w_FrictionJoint_setMaxForce },
	{ "getMaxForce", w_FrictionJoint_getMaxForce },
	{ "setMaxTorque", w_FrictionJoint_setMaxTorque },
	{ "getMaxTorque", w_FrictionJoint_getMaxTorque },
	// From Joint.
	{ "getType", w_Joint_getType },
	{ "getAnchors", w_Joint_getAnchors },
	{ "getReactionForce", w_Joint_getReactionForce },
	{ "getReactionTorque", w_Joint_getReactionTorque },
	{ "getCollideConnected", w_Joint_getCollideConnected },
	{ "destroy", w_Joint_destroy },
	{ 0, 0 }
};

extern "C" int luaopen_frictionjoint(lua_State *L)
{
	return luax_register_type(L, "FrictionJoint", functions);
}

} // box2d
} // physics
} // love
