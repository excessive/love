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

#ifndef LOVE_GRAPHICS_OPENGL_WRAP_PARTICLE_SYSTEM_H
#define LOVE_GRAPHICS_OPENGL_WRAP_PARTICLE_SYSTEM_H

// LOVE
#include "common/runtime.h"
#include "ParticleSystem.h"

namespace love
{
namespace graphics
{
namespace opengl
{

ParticleSystem *luax_checkparticlesystem(lua_State *L, int idx);
int w_ParticleSystem_clone(lua_State *L);
int w_ParticleSystem_setTexture(lua_State *L);
int w_ParticleSystem_getTexture(lua_State *L);
int w_ParticleSystem_setBufferSize(lua_State *L);
int w_ParticleSystem_getBufferSize(lua_State *L);
int w_ParticleSystem_setInsertMode(lua_State *L);
int w_ParticleSystem_getInsertMode(lua_State *L);
int w_ParticleSystem_setEmissionRate(lua_State *L);
int w_ParticleSystem_getEmissionRate(lua_State *L);
int w_ParticleSystem_setEmitterLifetime(lua_State *L);
int w_ParticleSystem_getEmitterLifetime(lua_State *L);
int w_ParticleSystem_setParticleLifetime(lua_State *L);
int w_ParticleSystem_getParticleLifetime(lua_State *L);
int w_ParticleSystem_setPosition(lua_State *L);
int w_ParticleSystem_getPosition(lua_State *L);
int w_ParticleSystem_setAreaSpread(lua_State *L);
int w_ParticleSystem_getAreaSpread(lua_State *L);
int w_ParticleSystem_setDirection(lua_State *L);
int w_ParticleSystem_getDirection(lua_State *L);
int w_ParticleSystem_setSpread(lua_State *L);
int w_ParticleSystem_getSpread(lua_State *L);
int w_ParticleSystem_setSpeed(lua_State *L);
int w_ParticleSystem_getSpeed(lua_State *L);
int w_ParticleSystem_setLinearAcceleration(lua_State *L);
int w_ParticleSystem_getLinearAcceleration(lua_State *L);
int w_ParticleSystem_setRadialAcceleration(lua_State *L);
int w_ParticleSystem_getRadialAcceleration(lua_State *L);
int w_ParticleSystem_setTangentialAcceleration(lua_State *L);
int w_ParticleSystem_getTangentialAcceleration(lua_State *L);
int w_ParticleSystem_setSizes(lua_State *L);
int w_ParticleSystem_getSizes(lua_State *L);
int w_ParticleSystem_setSizeVariation(lua_State *L);
int w_ParticleSystem_getSizeVariation(lua_State *L);
int w_ParticleSystem_setRotation(lua_State *L);
int w_ParticleSystem_getRotation(lua_State *L);
int w_ParticleSystem_setSpin(lua_State *L);
int w_ParticleSystem_getSpin(lua_State *L);
int w_ParticleSystem_setSpinVariation(lua_State *L);
int w_ParticleSystem_getSpinVariation(lua_State *L);
int w_ParticleSystem_setColors(lua_State *L);
int w_ParticleSystem_getColors(lua_State *L);
int w_ParticleSystem_setOffset(lua_State *L);
int w_ParticleSystem_getOffset(lua_State *L);
int w_ParticleSystem_getCount(lua_State *L);
int w_ParticleSystem_start(lua_State *L);
int w_ParticleSystem_stop(lua_State *L);
int w_ParticleSystem_pause(lua_State *L);
int w_ParticleSystem_reset(lua_State *L);
int w_ParticleSystem_emit(lua_State *L);
int w_ParticleSystem_isActive(lua_State *L);
int w_ParticleSystem_isPaused(lua_State *L);
int w_ParticleSystem_isStopped(lua_State *L);
int w_ParticleSystem_update(lua_State *L);
extern "C" int luaopen_particlesystem(lua_State *L);

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_WRAP_PARTICLE_SYSTEM_H
