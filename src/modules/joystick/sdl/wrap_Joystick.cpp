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

// LOVE
#include "wrap_Joystick.h"
#include "wrap_JoystickModule.h"

#include <vector>

namespace love
{
namespace joystick
{
namespace sdl
{

Joystick *luax_checkjoystick(lua_State *L, int idx)
{
	return luax_checktype<Joystick>(L, idx, "Joystick", JOYSTICK_JOYSTICK_T);
}

int w_Joystick_isConnected(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	luax_pushboolean(L, j->isConnected());
	return 1;
}

int w_Joystick_getName(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	lua_pushstring(L, j->getName());
	return 1;
}

int w_Joystick_getID(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);

	// IDs are 1-based in Lua.
	lua_pushinteger(L, j->getID() + 1);

	int instanceid = j->getInstanceID();
	if (instanceid >= 0)
		lua_pushinteger(L, instanceid + 1);
	else
		lua_pushnil(L);

	return 2;
}

int w_Joystick_getProductGUID(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	luax_pushstring(L, j->getProductGUID());
	return 1;
}

int w_Joystick_getAxisCount(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	lua_pushinteger(L, j->getAxisCount());
	return 1;
}

int w_Joystick_getButtonCount(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	lua_pushinteger(L, j->getButtonCount());
	return 1;
}

int w_Joystick_getHatCount(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	lua_pushinteger(L, j->getHatCount());
	return 1;
}

int w_Joystick_getAxis(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	int axisindex = luaL_checkint(L, 2) - 1;
	lua_pushnumber(L, j->getAxis(axisindex));
	return 1;
}

int w_Joystick_getAxes(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	std::vector<float> axes = j->getAxes();

	for (size_t i = 0; i < axes.size(); i++)
		lua_pushnumber(L, axes[i]);

	return (int) axes.size();
}

int w_Joystick_getHat(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	int hatindex = luaL_checkint(L, 2) - 1;

	Joystick::Hat h = j->getHat(hatindex);

	const char *direction = "";
	love::joystick::Joystick::getConstant(h, direction);

	lua_pushstring(L, direction);
	return 1;
}

int w_Joystick_isDown(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);

	luaL_checkinteger(L, 2);

	std::vector<int> buttons;
	for (int i = 2; i <= lua_gettop(L); i++)
		buttons.push_back(luaL_checkint(L, i));

	luax_pushboolean(L, j->isDown(buttons));
	return 1;
}

int w_Joystick_isGamepad(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);
	luax_pushboolean(L, j->isGamepad());
	return 1;
}

int w_Joystick_getGamepadAxis(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);

	const char *str = luaL_checkstring(L, 2);
	Joystick::GamepadAxis axis;

	if (!joystick::Joystick::getConstant(str, axis))
		return luaL_error(L, "Invalid gamepad axis: %s", str);

	lua_pushnumber(L, j->getGamepadAxis(axis));
	return 1;
}

int w_Joystick_isGamepadDown(lua_State *L)
{
	Joystick *j = luax_checkjoystick(L, 1);

	std::vector<Joystick::GamepadButton> buttons;
	buttons.reserve(lua_gettop(L) - 1);

	luaL_checkstring(L, 2);

	for (int i = 2; i <= lua_gettop(L); i++)
	{
		const char *str = luaL_checkstring(L, i);
		Joystick::GamepadButton button;

		if (!joystick::Joystick::getConstant(str, button))
			return luaL_error(L, "Invalid gamepad button: %s", str);

		buttons.push_back(button);
	}

	luax_pushboolean(L, j->isGamepadDown(buttons));
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "isConnected", w_Joystick_isConnected },
	{ "getName", w_Joystick_getName },
	{ "getID", w_Joystick_getID },
	{ "getProductGUID", w_Joystick_getProductGUID },
	{ "getAxisCount", w_Joystick_getAxisCount },
	{ "getButtonCount", w_Joystick_getButtonCount },
	{ "getHatCount", w_Joystick_getHatCount },
	{ "getAxis", w_Joystick_getAxis },
	{ "getAxes", w_Joystick_getAxes },
	{ "getHat", w_Joystick_getHat },
	{ "isDown", w_Joystick_isDown },
	{ "isGamepad", w_Joystick_isGamepad },
	{ "getGamepadAxis", w_Joystick_getGamepadAxis },
	{ "isGamepadDown", w_Joystick_isGamepadDown },

	// From wrap_JoystickModule.
	{ "getConnectedIndex", w_getIndex },
	{ "getGamepadMapping", w_getGamepadMapping },
	{ 0, 0 },
};

extern "C" int luaopen_joystick(lua_State *L)
{
	return luax_register_type(L, "Joystick", functions);
}

} // sdl
} // joystick
} // love
