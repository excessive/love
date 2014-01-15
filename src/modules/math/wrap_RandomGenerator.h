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

#ifndef LOVE_MATH_WRAP_RANDOM_GENERATOR_H
#define LOVE_MATH_WRAP_RANDOM_GENERATOR_H

// LOVE
#include "RandomGenerator.h"
#include "common/config.h"
#include "common/runtime.h"

namespace love
{
namespace math
{

// Helper functions.
RandomGenerator::Seed luax_checkrandomseed(lua_State *L, int idx);
int luax_getrandom(lua_State *L, int startidx, double r);

RandomGenerator *luax_checkrandomgenerator(lua_State *L, int idx);
int w_RandomGenerator_setSeed(lua_State *L);
int w_RandomGenerator_getSeed(lua_State *L);
int w_RandomGenerator_random(lua_State *L);
int w_RandomGenerator_randomNormal(lua_State *L);
extern "C" int luaopen_randomgenerator(lua_State *L);

} // math
} // love

#endif // LOVE_MATH_WRAP_RANDOM_GENERATOR_H
