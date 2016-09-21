/**
 * Copyright (c) 2006-2016 LOVE Development Team
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
#include "wrap_SpriteBatch.h"
#include "Image.h"
#include "Canvas.h"
#include "graphics/wrap_Texture.h"

// C++
#include <typeinfo>

namespace love
{
namespace graphics
{
namespace opengl
{

SpriteBatch *luax_checkspritebatch(lua_State *L, int idx)
{
	return luax_checktype<SpriteBatch>(L, idx, GRAPHICS_SPRITE_BATCH_ID);
}

static inline int w_SpriteBatch_add_or_set(lua_State *L, SpriteBatch *t, int startidx, int index)
{
	Quad *quad = nullptr;

	if (luax_istype(L, startidx, GRAPHICS_QUAD_ID))
	{
		quad = luax_totype<Quad>(L, startidx, GRAPHICS_QUAD_ID);
		startidx++;
	}
	else if (lua_isnil(L, startidx) && !lua_isnoneornil(L, startidx + 1))
		return luax_typerror(L, startidx, "Quad");

	float x  = (float) luaL_optnumber(L, startidx + 0, 0.0);
	float y  = (float) luaL_optnumber(L, startidx + 1, 0.0);
	float a  = (float) luaL_optnumber(L, startidx + 2, 0.0);
	float sx = (float) luaL_optnumber(L, startidx + 3, 1.0);
	float sy = (float) luaL_optnumber(L, startidx + 4, sx);
	float ox = (float) luaL_optnumber(L, startidx + 5, 0.0);
	float oy = (float) luaL_optnumber(L, startidx + 6, 0.0);
	float kx = (float) luaL_optnumber(L, startidx + 7, 0.0);
	float ky = (float) luaL_optnumber(L, startidx + 8, 0.0);

	luax_catchexcept(L, [&]() {
		if (quad)
			index = t->addq(quad, x, y, a, sx, sy, ox, oy, kx, ky, index);
		else
			index = t->add(x, y, a, sx, sy, ox, oy, kx, ky, index);
	});

	return index;
}

int w_SpriteBatch_add(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);

	int index = w_SpriteBatch_add_or_set(L, t, 2, -1);
	lua_pushinteger(L, index + 1);

	return 1;
}

int w_SpriteBatch_set(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	int index = (int) luaL_checknumber(L, 2) - 1;

	w_SpriteBatch_add_or_set(L, t, 3, index);

	return 0;
}

int w_SpriteBatch_clear(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	t->clear();
	return 0;
}

int w_SpriteBatch_flush(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	t->flush();
	return 0;
}

int w_SpriteBatch_setTexture(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	Texture *tex = luax_checktexture(L, 2);
	t->setTexture(tex);
	return 0;
}

int w_SpriteBatch_getTexture(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	Texture *tex = t->getTexture();

	// FIXME: big hack right here.
	if (typeid(*tex) == typeid(Image))
		luax_pushtype(L, GRAPHICS_IMAGE_ID, tex);
	else if (typeid(*tex) == typeid(Canvas))
		luax_pushtype(L, GRAPHICS_CANVAS_ID, tex);
	else
		return luaL_error(L, "Unable to determine texture type.");

	return 1;
}

int w_SpriteBatch_setColor(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	Color c;

	if (lua_gettop(L) <= 1)
	{
		t->setColor();
		return 0;
	}
	else if (lua_istable(L, 2))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 2, i);

		c.r = (unsigned char) (luaL_checknumber(L, -4) * 255.0);
		c.g = (unsigned char) (luaL_checknumber(L, -3) * 255.0);
		c.b = (unsigned char) (luaL_checknumber(L, -2) * 255.0);
		c.a = (unsigned char) (luaL_optnumber(L, -1, 1.0) * 255.0);

		lua_pop(L, 4);
	}
	else
	{
		c.r = (unsigned char) (luaL_checknumber(L, 2) * 255.0);
		c.g = (unsigned char) (luaL_checknumber(L, 3) * 255.0);
		c.b = (unsigned char) (luaL_checknumber(L, 4) * 255.0);
		c.a = (unsigned char) (luaL_optnumber(L, 5, 1.0) * 255.0);
	}

	t->setColor(c);

	return 0;
}

int w_SpriteBatch_getColor(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	const Color *color = t->getColor();

	// getColor returns null if no color is set.
	if (!color)
		return 0;

	lua_pushnumber(L, (lua_Number) color->r / 255.0);
	lua_pushnumber(L, (lua_Number) color->g / 255.0);
	lua_pushnumber(L, (lua_Number) color->b / 255.0);
	lua_pushnumber(L, (lua_Number) color->a / 255.0);

	return 4;
}

int w_SpriteBatch_getCount(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	lua_pushinteger(L, t->getCount());
	return 1;
}

int w_SpriteBatch_getBufferSize(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	lua_pushinteger(L, t->getBufferSize());
	return 1;
}

int w_SpriteBatch_attachAttribute(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);
	const char *name = luaL_checkstring(L, 2);
	Mesh *m = luax_checktype<Mesh>(L, 3, GRAPHICS_MESH_ID);

	luax_catchexcept(L, [&](){ t->attachAttribute(name, m); });
	return 0;
}

int w_SpriteBatch_setDrawRange(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);

	if (lua_isnoneornil(L, 2))
		t->setDrawRange();
	else
	{
		int start = (int) luaL_checknumber(L, 2) - 1;
		int count = (int) luaL_checknumber(L, 3);
		luax_catchexcept(L, [&](){ t->setDrawRange(start, count); });
	}

	return 0;
}

int w_SpriteBatch_getDrawRange(lua_State *L)
{
	SpriteBatch *t = luax_checkspritebatch(L, 1);

	int start = 0;
	int count = 1;
	if (!t->getDrawRange(start, count))
		return 0;

	lua_pushnumber(L, start + 1);
	lua_pushnumber(L, count);
	return 2;
}

static const luaL_Reg w_SpriteBatch_functions[] =
{
	{ "add", w_SpriteBatch_add },
	{ "set", w_SpriteBatch_set },
	{ "clear", w_SpriteBatch_clear },
	{ "flush", w_SpriteBatch_flush },
	{ "setTexture", w_SpriteBatch_setTexture },
	{ "getTexture", w_SpriteBatch_getTexture },
	{ "setColor", w_SpriteBatch_setColor },
	{ "getColor", w_SpriteBatch_getColor },
	{ "getCount", w_SpriteBatch_getCount },
	{ "getBufferSize", w_SpriteBatch_getBufferSize },
	{ "attachAttribute", w_SpriteBatch_attachAttribute },
	{ "setDrawRange", w_SpriteBatch_setDrawRange },
	{ "getDrawRange", w_SpriteBatch_getDrawRange },
	{ 0, 0 }
};

extern "C" int luaopen_spritebatch(lua_State *L)
{
	return luax_register_type(L, GRAPHICS_SPRITE_BATCH_ID, "SpriteBatch", w_SpriteBatch_functions, nullptr);
}

} // opengl
} // graphics
} // love
