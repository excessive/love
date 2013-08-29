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

#include "Graphics.h"
#include "wrap_Canvas.h"

namespace love
{
namespace graphics
{
namespace opengl
{

Canvas *luax_checkcanvas(lua_State *L, int idx)
{
	return luax_checktype<Canvas>(L, idx, "Canvas", GRAPHICS_CANVAS_T);
}

int w_Canvas_renderTo(lua_State *L)
{
	// As startGrab() clears the framebuffer, better not allow
	// grabbing inside another grabbing
	if (Canvas::current != NULL)
	{
		Canvas::bindDefaultCanvas();
		return luaL_error(L, "Current render target not the default canvas!");
	}

	Canvas *canvas = luax_checkcanvas(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);

	try
	{
		canvas->startGrab();
	}
	catch (love::Exception &e)
	{
		return luaL_error(L, "%s", e.what());
	}
	lua_settop(L, 2); // make sure the function is on top of the stack
	lua_call(L, 0, 0);
	canvas->stopGrab();

	return 0;
}

int w_Canvas_getImageData(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	love::image::Image *image = luax_getmodule<love::image::Image>(L, "image", MODULE_IMAGE_T);
	love::image::ImageData *img = canvas->getImageData(image);
	luax_pushtype(L, "ImageData", IMAGE_IMAGE_DATA_T, img);
	return 1;
}

int w_Canvas_getPixel(lua_State * L)
{
	Canvas * canvas = luax_checkcanvas(L, 1);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	unsigned char c[4];
	try
	{
		canvas->getPixel(c, x, y);
	}
	catch (love::Exception & e)
	{
		return luaL_error(L, "%s", e.what());
	}
	lua_pushnumber(L, c[0]);
	lua_pushnumber(L, c[1]);
	lua_pushnumber(L, c[2]);
	lua_pushnumber(L, c[3]);
	return 4;
}

int w_Canvas_setFilter(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);

	Image::Filter f;

	const char *minstr = luaL_checkstring(L, 2);
	const char *magstr = luaL_optstring(L, 3, minstr);

	if (!Image::getConstant(minstr, f.min))
		return luaL_error(L, "Invalid filter mode: %s", minstr);
	if (!Image::getConstant(magstr, f.mag))
		return luaL_error(L, "Invalid filter mode: %s", magstr);

	f.anisotropy = (float) luaL_optnumber(L, 4, 1.0);

	canvas->setFilter(f);

	return 0;

}

int w_Canvas_getFilter(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	const Image::Filter f = canvas->getFilter();

	const char *minstr;
	const char *magstr;
	Image::getConstant(f.min, minstr);
	Image::getConstant(f.mag, magstr);

	lua_pushstring(L, minstr);
	lua_pushstring(L, magstr);
	lua_pushnumber(L, f.anisotropy);

	return 3;
}

int w_Canvas_setWrap(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);

	Image::Wrap w;

	const char *sstr = luaL_checkstring(L, 2);
	const char *tstr = luaL_optstring(L, 3, sstr);

	if (!Image::getConstant(sstr, w.s))
		return luaL_error(L, "Invalid wrap mode: %s", sstr);
	if (!Image::getConstant(tstr, w.t))
		return luaL_error(L, "Invalid wrap mode, %s", tstr);

	canvas->setWrap(w);

	return 0;
}

int w_Canvas_getWrap(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	const Image::Wrap w = canvas->getWrap();

	const char *wrap_s;
	const char *wrap_t;
	Image::getConstant(w.s, wrap_s);
	Image::getConstant(w.t, wrap_t);

	lua_pushstring(L, wrap_s);
	lua_pushstring(L, wrap_t);

	return 2;
}

int w_Canvas_clear(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	Color c;
	if (lua_isnoneornil(L, 2))
	{
		c.set(0, 0, 0, 0);
	}
	else if (lua_istable(L, 2))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 2, i);

		c.r = (unsigned char)luaL_checkinteger(L, -4);
		c.g = (unsigned char)luaL_checkinteger(L, -3);
		c.b = (unsigned char)luaL_checkinteger(L, -2);
		c.a = (unsigned char)luaL_optinteger(L, -1, 255);

		lua_pop(L, 4);
	}
	else
	{
		c.r = (unsigned char)luaL_checkinteger(L, 2);
		c.g = (unsigned char)luaL_checkinteger(L, 3);
		c.b = (unsigned char)luaL_checkinteger(L, 4);
		c.a = (unsigned char)luaL_optinteger(L, 5, 255);
	}
	canvas->clear(c);

	return 0;
}

int w_Canvas_getWidth(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	lua_pushnumber(L, canvas->getWidth());
	return 1;
}

int w_Canvas_getHeight(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	lua_pushnumber(L, canvas->getHeight());
	return 1;
}

int w_Canvas_getDimensions(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	lua_pushnumber(L, canvas->getWidth());
	lua_pushnumber(L, canvas->getHeight());
	return 2;
}

int w_Canvas_getType(lua_State *L)
{
	Canvas *canvas = luax_checkcanvas(L, 1);
	Canvas::TextureType type = canvas->getTextureType();
	const char *str;
	Canvas::getConstant(type, str);
	lua_pushstring(L, str);
	return 1;
}

static const luaL_Reg functions[] =
{
	{ "renderTo", w_Canvas_renderTo },
	{ "getImageData", w_Canvas_getImageData },
	{ "getPixel", w_Canvas_getPixel },
	{ "setFilter", w_Canvas_setFilter },
	{ "getFilter", w_Canvas_getFilter },
	{ "setWrap", w_Canvas_setWrap },
	{ "getWrap", w_Canvas_getWrap },
	{ "clear", w_Canvas_clear },
	{ "getWidth", w_Canvas_getWidth },
	{ "getHeight", w_Canvas_getHeight },
	{ "getDimensions", w_Canvas_getDimensions },
	{ "getType", w_Canvas_getType },
	{ 0, 0 }
};

extern "C" int luaopen_canvas(lua_State *L)
{
	return luax_register_type(L, "Canvas", functions);
}

} // opengl
} // graphics
} // love
