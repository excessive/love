/**
 * Copyright (c) 2006-2015 LOVE Development Team
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
#include "wrap_Joint.h"
#include "common/StringMap.h"
#include "Body.h"

namespace love
{
namespace physics
{
namespace box2d
{

void luax_pushjoint(lua_State *L, Joint *j)
{
	if (j == nullptr)
		return lua_pushnil(L);

	switch (j->getType())
	{
	case Joint::JOINT_DISTANCE:
		return luax_pushtype(L, "DistanceJoint", PHYSICS_DISTANCE_JOINT_T, j);
	case Joint::JOINT_REVOLUTE:
		return luax_pushtype(L, "RevoluteJoint", PHYSICS_REVOLUTE_JOINT_T, j);
	case Joint::JOINT_PRISMATIC:
		return luax_pushtype(L, "PrismaticJoint", PHYSICS_PRISMATIC_JOINT_T, j);
	case Joint::JOINT_MOUSE:
		return luax_pushtype(L, "MouseJoint", PHYSICS_MOUSE_JOINT_T, j);
	case Joint::JOINT_PULLEY:
		return luax_pushtype(L, "PulleyJoint", PHYSICS_PULLEY_JOINT_T, j);
	case Joint::JOINT_GEAR:
		return luax_pushtype(L, "GearJoint", PHYSICS_GEAR_JOINT_T, j);
	case Joint::JOINT_FRICTION:
		return luax_pushtype(L, "FrictionJoint", PHYSICS_FRICTION_JOINT_T, j);
	case Joint::JOINT_WELD:
		return luax_pushtype(L, "WeldJoint", PHYSICS_WELD_JOINT_T, j);
	case Joint::JOINT_WHEEL:
		return luax_pushtype(L, "WheelJoint", PHYSICS_WHEEL_JOINT_T, j);
	case Joint::JOINT_ROPE:
		return luax_pushtype(L, "RopeJoint", PHYSICS_ROPE_JOINT_T, j);
	case Joint::JOINT_MOTOR:
		return luax_pushtype(L, "MotorJoint", PHYSICS_MOTOR_JOINT_T, j);
	default:
		return lua_pushnil(L);
	}
}

Joint *luax_checkjoint(lua_State *L, int idx)
{
	Joint *t = luax_checktype<Joint>(L, idx, "Joint", PHYSICS_JOINT_T);
	if (!t->isValid())
		luaL_error(L, "Attempt to use destroyed joint.");
	return t;
}

int w_Joint_getType(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	const char *type = "";
	Joint::getConstant(t->getType(), type);
	lua_pushstring(L, type);
	return 1;
}

int w_Joint_getBodies(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	Body *b1 = nullptr;
	Body *b2 = nullptr;

	luax_catchexcept(L, [&]() {
		b1 = t->getBodyA();
		b2 = t->getBodyB();
	});

	luax_pushtype(L, "Body", PHYSICS_BODY_T, b1);
	luax_pushtype(L, "Body", PHYSICS_BODY_T, b2);
	return 2;
}

int w_Joint_getAnchors(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	lua_remove(L, 1);
	return t->getAnchors(L);
}

int w_Joint_getReactionForce(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	lua_remove(L, 1);
	return t->getReactionForce(L);
}

int w_Joint_getReactionTorque(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	float inv_dt = (float)luaL_checknumber(L, 2);
	lua_pushnumber(L, t->getReactionTorque(inv_dt));
	return 1;
}

int w_Joint_getCollideConnected(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	luax_pushboolean(L, t->getCollideConnected());
	return 1;
}

int w_Joint_setUserData(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	lua_remove(L, 1);
	return t->setUserData(L);
}

int w_Joint_getUserData(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	lua_remove(L, 1);
	return t->getUserData(L);
}

int w_Joint_destroy(lua_State *L)
{
	Joint *t = luax_checkjoint(L, 1);
	luax_catchexcept(L, [&](){ t->destroyJoint(); });
	return 0;
}

int w_Joint_isDestroyed(lua_State *L)
{
	Joint *t = luax_checktype<Joint>(L, 1, "Joint", PHYSICS_JOINT_T);
	luax_pushboolean(L, !t->isValid());
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "getType", w_Joint_getType },
	{ "getBodies", w_Joint_getBodies },
	{ "getAnchors", w_Joint_getAnchors },
	{ "getReactionForce", w_Joint_getReactionForce },
	{ "getReactionTorque", w_Joint_getReactionTorque },
	{ "getCollideConnected", w_Joint_getCollideConnected },
	{ "setUserData", w_Joint_setUserData },
	{ "getUserData", w_Joint_getUserData },
	{ "destroy", w_Joint_destroy },
	{ "isDestroyed", w_Joint_isDestroyed },
	{ 0, 0 }
};

extern "C" int luaopen_joint(lua_State *L)
{
	return luax_register_type(L, "Joint", functions);
}

} // box2d
} // physics
} // love
