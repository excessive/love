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
#include "wrap_Cursor.h"
#include "Cursor.h"

namespace love
{
namespace mouse
{

int w_getType(lua_State *L)
{
	mouse::Cursor *cursor = luax_checktype<mouse::Cursor>(L, 1, "Cursor", MOUSE_CURSOR_T);

	Cursor::CursorType ctype = cursor->getType();
	const char *typestr = 0;

	if (ctype == Cursor::CURSORTYPE_IMAGE)
		mouse::Cursor::getConstant(ctype, typestr);
	else if (ctype == Cursor::CURSORTYPE_SYSTEM)
	{
		Cursor::SystemCursor systype = cursor->getSystemType();
		mouse::Cursor::getConstant(systype, typestr);
	}

	if (!typestr)
		return luaL_error(L, "Unknown cursor type.");

	lua_pushstring(L, typestr);
	return 1;
};

static const luaL_Reg functions[] =
{
	{ "getType", w_getType },
	{ 0, 0 },
};

extern "C" int luaopen_cursor(lua_State *L)
{
	return luax_register_type(L, "Cursor", functions);
}

} // mouse
} // love