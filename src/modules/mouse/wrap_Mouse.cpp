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
#include "wrap_Mouse.h"
#include "wrap_Cursor.h"
#include "common/config.h"

#include "sdl/Mouse.h"

namespace love
{
namespace mouse
{

static Mouse *instance = 0;

int w_newCursor(lua_State *L)
{
	Cursor *cursor = 0;

	if (lua_isstring(L, 1))
	{
		// System cursor type.
		const char *str = luaL_checkstring(L, 1);
		Cursor::SystemCursor systemCursor;

		if (!Cursor::getConstant(str, systemCursor))
			return luaL_error(L, "Invalid cursor type %s", str);

		try
		{
			cursor = instance->newCursor(systemCursor);
		}
		catch (love::Exception &e)
		{
			return luaL_error(L, "%s", e.what());
		}
	}
	else
	{
		// Custom image.
		love::image::ImageData *data = luax_checktype<love::image::ImageData>(L, 1, "ImageData", IMAGE_IMAGE_DATA_T);
		int hotx = luaL_optint(L, 2, 0);
		int hoty = luaL_optint(L, 3, 0);

		try
		{
			cursor = instance->newCursor(data, hotx, hoty);
		}
		catch (love::Exception &e)
		{
			return luaL_error(L, "%s", e.what());
		}
	}

	luax_newtype(L, "Cursor", MOUSE_CURSOR_T, (void *) cursor);
	return 1;
}

int w_setCursor(lua_State *L)
{
	// Revert to the default system cursor if no argument is given.
	if (lua_isnoneornil(L, 1))
	{
		instance->setCursor();
		return 0;
	}

	Cursor *cursor = luax_checktype<Cursor>(L, 1, "Cursor", MOUSE_CURSOR_T);
	instance->setCursor(cursor);
	return 0;
}

int w_getCursor(lua_State *L)
{
	Cursor *cursor = instance->getCursor();

	if (cursor)
	{
		cursor->retain();
		luax_newtype(L, "Cursor", MOUSE_CURSOR_T, (void *) cursor);
	}
	else
		lua_pushnil(L);

	return 1;
}

int w_getX(lua_State *L)
{
	lua_pushnumber(L, instance->getX());
	return 1;
}

int w_getY(lua_State *L)
{
	lua_pushnumber(L, instance->getY());
	return 1;
}

int w_getPosition(lua_State *L)
{
	int x, y;
	instance->getPosition(x, y);
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

int w_setX(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	instance->setX(x);
	return 0;
}

int w_setY(lua_State *L)
{
	int y = luaL_checkint(L, 1);
	instance->setY(y);
	return 0;
}

int w_setPosition(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	instance->setPosition(x, y);
	return 0;
}

int w_isDown(lua_State *L)
{
	Mouse::Button b;
	unsigned int num = lua_gettop(L);
	Mouse::Button *buttonlist = new Mouse::Button[num+1];
	unsigned int counter = 0;

	for (unsigned int i = 0; i < num; i++)
	{
		if (Mouse::getConstant(luaL_checkstring(L, i+1), b))
			buttonlist[counter++] = b;
	}
	buttonlist[counter] = Mouse::BUTTON_MAX_ENUM;

	luax_pushboolean(L, instance->isDown(buttonlist));
	delete[] buttonlist;
	return 1;
}

int w_setVisible(lua_State *L)
{
	bool b = luax_toboolean(L, 1);
	instance->setVisible(b);
	return 0;
}

int w_isVisible(lua_State *L)
{
	luax_pushboolean(L, instance->isVisible());
	return 1;
}

int w_setGrabbed(lua_State *L)
{
	bool b = luax_toboolean(L, 1);
	instance->setGrabbed(b);
	return 0;
}

int w_isGrabbed(lua_State *L)
{
	luax_pushboolean(L, instance->isGrabbed());
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "newCursor", w_newCursor },
	{ "setCursor", w_setCursor },
	{ "getCursor", w_getCursor },
	{ "getX", w_getX },
	{ "getY", w_getY },
	{ "setX", w_setX },
	{ "setY", w_setY },
	{ "setPosition", w_setPosition },
	{ "isDown", w_isDown },
	{ "setVisible", w_setVisible },
	{ "isVisible", w_isVisible },
	{ "getPosition", w_getPosition },
	{ "setGrabbed", w_setGrabbed },
	{ "isGrabbed", w_isGrabbed },
	{ 0, 0 }
};

// Types for this module.
static const lua_CFunction types[] =
{
	luaopen_cursor,
	0,
};

extern "C" int luaopen_love_mouse(lua_State *L)
{
	if (instance == 0)
	{
		try
		{
			instance = new love::mouse::sdl::Mouse();
		}
		catch(Exception &e)
		{
			return luaL_error(L, e.what());
		}
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "mouse";
	w.flags = MODULE_T;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // mouse
} // love
