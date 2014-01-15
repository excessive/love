/**
 * Copyright (c) 2006-2014 LOVE Development Team
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

#ifndef LOVE_PHYSICS_BOX2D_WRAP_WHEEL_JOINT_H
#define LOVE_PHYSICS_BOX2D_WRAP_WHEEL_JOINT_H

// LOVE
#include "common/runtime.h"
#include "wrap_Joint.h"
#include "WheelJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{

WheelJoint *luax_checkwheeljoint(lua_State *L, int idx);
int w_WheelJoint_getJointTranslation(lua_State *L);
int w_WheelJoint_getJointSpeed(lua_State *L);
int w_WheelJoint_setMotorEnabled(lua_State *L);
int w_WheelJoint_isMotorEnabled(lua_State *L);
int w_WheelJoint_setMotorSpeed(lua_State *L);
int w_WheelJoint_getMotorSpeed(lua_State *L);
int w_WheelJoint_setMaxMotorTorque(lua_State *L);
int w_WheelJoint_getMaxMotorTorque(lua_State *L);
int w_WheelJoint_getMotorTorque(lua_State *L);
int w_WheelJoint_setSpringFrequency(lua_State *L);
int w_WheelJoint_getSpringFrequency(lua_State *L);
int w_WheelJoint_setSpringDampingRatio(lua_State *L);
int w_WheelJoint_getSpringDampingRatio(lua_State *L);
extern "C" int luaopen_wheeljoint(lua_State *L);

} // box2d
} // physics
} // love

#endif // LOVE_PHYSICS_BOX2D_WRAP_WHEEL_JOINT_H
