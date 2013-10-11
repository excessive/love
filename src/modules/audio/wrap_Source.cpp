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

#include "wrap_Source.h"

namespace love
{
namespace audio
{

Source *luax_checksource(lua_State *L, int idx)
{
	return luax_checktype<Source>(L, idx, "Source", AUDIO_SOURCE_T);
}

int w_Source_play(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->play();
	return 0;
}

int w_Source_stop(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->stop();
	return 0;
}

int w_Source_pause(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->pause();
	return 0;
}

int w_Source_resume(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->resume();
	return 0;
}

int w_Source_rewind(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->rewind();
	return 0;
}

int w_Source_setPitch(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float p = (float)luaL_checknumber(L, 2);
	t->setPitch(p);
	return 0;
}

int w_Source_getPitch(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	lua_pushnumber(L, t->getPitch());
	return 1;
}

int w_Source_setVolume(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float p = (float)luaL_checknumber(L, 2);
	t->setVolume(p);
	return 0;
}

int w_Source_getVolume(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	lua_pushnumber(L, t->getVolume());
	return 1;
}

int w_Source_seek(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float offset = (float)luaL_checknumber(L, 2);
	if (offset < 0)
		return luaL_argerror(L, 2, "can't seek to a negative position");

	Source::Unit u = Source::UNIT_SECONDS;
	const char *unit = lua_isnoneornil(L, 3) ? 0 : lua_tostring(L, 3);
	if (unit && !t->getConstant(unit, u))
		return luaL_error(L, "Invalid Source time unit: %s", unit);

	t->seek(offset, u);
	return 0;
}

int w_Source_tell(lua_State *L)
{
	Source *t = luax_checksource(L, 1);

	Source::Unit u = Source::UNIT_SECONDS;
	const char *unit = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);
	if (unit && !t->getConstant(unit, u))
		return luaL_error(L, "Invalid Source time unit: %s", unit);

	lua_pushnumber(L, t->tell(u));
	return 1;
}

int w_Source_setPosition(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	v[0] = (float)luaL_checknumber(L, 2);
	v[1] = (float)luaL_checknumber(L, 3);
	v[2] = (float)luaL_optnumber(L, 4, 0);
	t->setPosition(v);
	return 0;
}

int w_Source_getPosition(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	t->getPosition(v);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);
	return 3;
}

int w_Source_setVelocity(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	v[0] = (float)luaL_checknumber(L, 2);
	v[1] = (float)luaL_checknumber(L, 3);
	v[2] = (float)luaL_optnumber(L, 4, 0);
	t->setVelocity(v);
	return 0;
}

int w_Source_getVelocity(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	t->getVelocity(v);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);
	return 3;
}

int w_Source_setDirection(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	v[0] = (float)luaL_checknumber(L, 2);
	v[1] = (float)luaL_checknumber(L, 3);
	v[2] = (float)luaL_optnumber(L, 4, 0);
	t->setDirection(v);
	return 0;
}

int w_Source_getDirection(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float v[3];
	t->getDirection(v);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);
	return 3;
}

int w_Source_setCone(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float innerAngle = (float) luaL_checknumber(L, 2);
	float outerAngle = (float) luaL_checknumber(L, 3);
	float outerVolume = (float) luaL_optnumber(L, 4, 0.0);
	t->setCone(innerAngle, outerAngle, outerVolume);
	return 0;
}

int w_Source_getCone(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float innerAngle, outerAngle, outerVolume;
	t->getCone(innerAngle, outerAngle, outerVolume);
	lua_pushnumber(L, innerAngle);
	lua_pushnumber(L, outerAngle);
	lua_pushnumber(L, outerVolume);
	return 3;
}

int w_Source_setRelativePosition(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->setRelativePosition(luax_toboolean(L, 2));
	return 0;
}

int w_Source_hasRelativePosition(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, t->hasRelativePosition());
	return 1;
}

int w_Source_setLooping(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	t->setLooping(luax_toboolean(L, 2));
	return 0;
}

int w_Source_isLooping(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, t->isLooping());
	return 1;
}

int w_Source_isStopped(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, t->isStopped());
	return 1;
}

int w_Source_isPaused(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, t->isPaused());
	return 1;
}

int w_Source_isPlaying(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, !t->isStopped() && !t->isPaused());
	return 1;
}

int w_Source_isStatic(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	luax_pushboolean(L, t->isStatic());
	return 1;
}

int w_Source_setVolumeLimits(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float vmin = (float)luaL_checknumber(L, 2);
	float vmax = (float)luaL_checknumber(L, 3);
	if (vmin < .0f || vmin > 1.f || vmax < .0f || vmax > 1.f)
		return luaL_error(L, "Invalid volume limits: [%f:%f]. Must be in [0:1]", vmin, vmax);
	t->setMinVolume(vmin);
	t->setMaxVolume(vmax);
	return 0;
}

int w_Source_getVolumeLimits(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	lua_pushnumber(L, t->getMinVolume());
	lua_pushnumber(L, t->getMaxVolume());
	return 2;
}

int w_Source_setAttenuationDistances(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float dref = (float)luaL_checknumber(L, 2);
	float dmax = (float)luaL_checknumber(L, 3);
	if (dref < .0f || dmax < .0f)
		return luaL_error(L, "Invalid distances: %f, %f. Must be > 0", dref, dmax);
	t->setReferenceDistance(dref);
	t->setMaxDistance(dmax);
	return 0;
}

int w_Source_getAttenuationDistances(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	lua_pushnumber(L, t->getReferenceDistance());
	lua_pushnumber(L, t->getMaxDistance());
	return 2;
}

int w_Source_setRolloff(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	float rolloff = (float)luaL_checknumber(L, 2);
	if (rolloff < .0f)
		return luaL_error(L, "Invalid rolloff: %f. Must be > 0.", rolloff);
	t->setRolloffFactor(rolloff);
	return 0;
}

int w_Source_getRolloff(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	lua_pushnumber(L, t->getRolloffFactor());
	return 1;
}

int w_Source_getType(lua_State *L)
{
	Source *t = luax_checksource(L, 1);
	Source::Type stype = t->getType();
	const char *typestr;

	if (!Source::getConstant(stype, typestr))
		return luaL_error(L, "Unknown Source type.");

	lua_pushstring(L, typestr);
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "play", w_Source_play },
	{ "stop", w_Source_stop },
	{ "pause", w_Source_pause },
	{ "resume", w_Source_resume },
	{ "rewind", w_Source_rewind },

	{ "setPitch", w_Source_setPitch },
	{ "getPitch", w_Source_getPitch },
	{ "setVolume", w_Source_setVolume },
	{ "getVolume", w_Source_getVolume },
	{ "seek", w_Source_seek },
	{ "tell", w_Source_tell },
	{ "setPosition", w_Source_setPosition },
	{ "getPosition", w_Source_getPosition },
	{ "setVelocity", w_Source_setVelocity },
	{ "getVelocity", w_Source_getVelocity },
	{ "setDirection", w_Source_setDirection },
	{ "getDirection", w_Source_getDirection },
	{ "setCone", w_Source_setCone },
	{ "getCone", w_Source_getCone },

	{ "setRelativePosition", w_Source_setRelativePosition },
	{ "hasRelativePosition", w_Source_hasRelativePosition },

	{ "setLooping", w_Source_setLooping },
	{ "isLooping", w_Source_isLooping },
	{ "isStopped", w_Source_isStopped },
	{ "isPaused", w_Source_isPaused },
	{ "isPlaying", w_Source_isPlaying },
	{ "isStatic", w_Source_isStatic },

	{ "setVolumeLimits", w_Source_setVolumeLimits },
	{ "getVolumeLimits", w_Source_getVolumeLimits },
	{ "setAttenuationDistances", w_Source_setAttenuationDistances },
	{ "getAttenuationDistances", w_Source_getAttenuationDistances },
	{ "setRolloff", w_Source_setRolloff},
	{ "getRolloff", w_Source_getRolloff},

	{ "getType", w_Source_getType },

	{ 0, 0 }
};

extern "C" int luaopen_source(lua_State *L)
{
	return luax_register_type(L, "Source", functions);
}

} // audio
} // love
