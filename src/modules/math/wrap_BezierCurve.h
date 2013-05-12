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

#ifndef LOVE_MATH_WRAP_BEZIER_CURVE_H
#define LOVE_MATH_WRAP_BEZIER_CURVE_H

// LOVE
#include "BezierCurve.h"
#include "common/runtime.h"

namespace love
{
namespace math
{

BezierCurve *luax_checkbeziercurve(lua_State *L, int idx);
int w_BezierCurve_getDegree(lua_State *L);
int w_BezierCurve_getControlPoint(lua_State *L);
int w_BezierCurve_setControlPoint(lua_State *L);
int w_BezierCurve_insertControlPoint(lua_State *L);
int w_BezierCurve_eval(lua_State *L);
int w_BezierCurve_render(lua_State *L);
extern "C" int luaopen_beziercurve(lua_State *L);

} // math
} // love

#endif // LOVE_MATH_WRAP_RANDOM_GENERATOR_H
