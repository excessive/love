/**
* Copyright (c) 2006-2011 LOVE Development Team
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

#include "wrap_WheelJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{
	WheelJoint * luax_checkwheeljoint(lua_State * L, int idx)
	{
		return luax_checktype<WheelJoint>(L, idx, "WheelJoint", PHYSICS_WHEEL_JOINT_T);
	}

	int w_WheelJoint_getJointTranslation(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getJointTranslation());
		return 1;
	}

	int w_WheelJoint_getJointSpeed(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getJointSpeed());
		return 1;
	}

	int w_WheelJoint_enableMotor(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		bool arg1 = luax_toboolean(L, 2);
		t->enableMotor(arg1);
		return 0;
	}

	int w_WheelJoint_isMotorEnabled(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		luax_pushboolean(L, t->isMotorEnabled());
		return 1;
	}

	int w_WheelJoint_setMotorSpeed(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setMotorSpeed(arg1);
		return 0;
	}

	int w_WheelJoint_getMotorSpeed(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getMotorSpeed());
		return 1;
	}

	int w_WheelJoint_setMaxMotorTorque(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setMaxMotorTorque(arg1);
		return 0;
	}

	int w_WheelJoint_getMaxMotorTorque(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getMaxMotorTorque());
		return 1;
	}

	int w_WheelJoint_getMotorTorque(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		float inv_dt = (float)luaL_checknumber(L, 2);
		lua_pushnumber(L, t->getMotorTorque(inv_dt));
		return 1;
	}

	int w_WheelJoint_setSpringFrequencyHz(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setSpringFrequencyHz(arg1);
		return 0;
	}

	int w_WheelJoint_getSpringFrequencyHz(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getSpringFrequencyHz());
		return 1;
	}

	int w_WheelJoint_setSpringDampingRatio(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setSpringDampingRatio(arg1);
		return 0;
	}

	int w_WheelJoint_getSpringDampingRatio(lua_State * L)
	{
		WheelJoint * t = luax_checkwheeljoint(L, 1);
		lua_pushnumber(L, t->getSpringDampingRatio());
		return 1;
	}

	static const luaL_Reg functions[] = {
		{ "getJointTranslation", w_WheelJoint_getJointTranslation },
		{ "getJointSpeed", w_WheelJoint_getJointSpeed },
		{ "enableMotor", w_WheelJoint_enableMotor },
		{ "isMotorEnabled", w_WheelJoint_isMotorEnabled },
		{ "setMotorSpeed", w_WheelJoint_setMotorSpeed },
		{ "getMotorSpeed", w_WheelJoint_getMotorSpeed },
		{ "setMaxMotorTorque", w_WheelJoint_setMaxMotorTorque },
		{ "getMaxMotorTorque", w_WheelJoint_getMaxMotorTorque },
		{ "getMotorTorque", w_WheelJoint_getMotorTorque },
		{ "setSpringFrequencyHz", w_WheelJoint_setSpringFrequencyHz },
		{ "getSpringFrequencyHz", w_WheelJoint_getSpringFrequencyHz },
		{ "setSpringDampingRatio", w_WheelJoint_setSpringDampingRatio },
		{ "getSpringDampingRatio", w_WheelJoint_getSpringDampingRatio },
		// From Joint.
		{ "getType", w_Joint_getType },
		{ "getAnchors", w_Joint_getAnchors },
		{ "getReactionForce", w_Joint_getReactionForce },
		{ "getReactionTorque", w_Joint_getReactionTorque },
		{ "getCollideConnected", w_Joint_getCollideConnected },
		{ "destroy", w_Joint_destroy },
		{ 0, 0 }
	};

	int luaopen_wheeljoint(lua_State * L)
	{
		return luax_register_type(L, "WheelJoint", functions);
	}

} // box2d
} // physics
} // love
