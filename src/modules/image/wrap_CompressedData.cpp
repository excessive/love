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

#include "wrap_CompressedData.h"
#include "common/wrap_Data.h"

namespace love
{
namespace image
{

CompressedData *luax_checkcompresseddata(lua_State *L, int idx)
{
	return luax_checktype<CompressedData>(L, idx, "CompressedData", IMAGE_COMPRESSED_DATA_T);
}

int w_CompressedData_getString(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);

	// CompressedData's data isn't contiguous in memory, so we need to copy it
	// all to a single block before sending the string.

	size_t totalsize = 0;

	for (int i = 0; i < t->getNumMipmaps(); i++)
		totalsize += t->getSize(i);

	if (totalsize == 0)
	{
		lua_pushstring(L, "");
		return 1;
	}

	char *datastr = 0;
	try
	{
		datastr = new char[totalsize];
	}
	catch (std::exception &)
	{
		return luaL_error(L, "Out of memory.");
	}

	size_t curpos = 0;
	for (int i = 0; i < t->getNumMipmaps(); i++)
	{
		memcpy(&datastr[curpos], t->getData(i), t->getSize(i));
		curpos += t->getSize(i);
	}

	lua_pushlstring(L, datastr, totalsize);

	delete[] datastr;

	return 1;
}

int w_CompressedData_getSize(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);

	size_t totalsize = 0;

	for (int i = 0; i < t->getNumMipmaps(); i++)
		totalsize += t->getSize(i);

	lua_pushnumber(L, (lua_Number) totalsize);
	
	return 1;
}

int w_CompressedData_getWidth(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);
	int miplevel = luaL_optinteger(L, 2, 1);
	int width = 0;
	try
	{
		width = t->getWidth(miplevel >= 1 ? miplevel - 1 : 0);
	}
	catch (love::Exception &e)
	{
		return luaL_error(L, "%s", e.what());
	}
	lua_pushinteger(L, width);
	return 1;
}

int w_CompressedData_getHeight(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);
	int miplevel = luaL_optinteger(L, 2, 1);
	int height = 0;
	try
	{
		height = t->getHeight(miplevel >= 1 ? miplevel - 1 : 0);
	}
	catch (love::Exception &e)
	{
		return luaL_error(L, "%s", e.what());
	}
	lua_pushinteger(L, height);
	return 1;
}

int w_CompressedData_getDimensions(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);
	int miplevel = luaL_optinteger(L, 2, 1);
	int width = 0, height = 0;
	try
	{
		width = t->getWidth(miplevel >= 1 ? miplevel - 1 : 0);
		height = t->getHeight(miplevel >= 1 ? miplevel - 1 : 0);
	}
	catch (love::Exception &e)
	{
		return luaL_error(L, "%s", e.what());
	}
	lua_pushinteger(L, width);
	lua_pushinteger(L, height);
	return 2;
}

int w_CompressedData_getNumMipmaps(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);
	lua_pushinteger(L, t->getNumMipmaps());
	return 1;
}

int w_CompressedData_getType(lua_State *L)
{
	CompressedData *t = luax_checkcompresseddata(L, 1);

	image::CompressedData::TextureType type = t->getType();
	const char *str;

	if (image::CompressedData::getConstant(type, str))
		lua_pushstring(L, str);
	else
		lua_pushstring(L, "unknown");

	return 1;
}

static const luaL_Reg functions[] =
{
	// Data
	// CompressedData's data is not in contiguous memory, so it needs custom functions.
	{ "getString", w_CompressedData_getString },
	{ "getSize", w_CompressedData_getSize },

	{ "getWidth", w_CompressedData_getWidth },
	{ "getHeight", w_CompressedData_getHeight },
	{ "getDimensions", w_CompressedData_getDimensions },
	{ "getNumMipmaps", w_CompressedData_getNumMipmaps },
	{ "getType", w_CompressedData_getType },
	{ 0, 0 },
};

extern "C" int luaopen_compresseddata(lua_State *L)
{
	return luax_register_type(L, "CompressedData", functions);
}

} // image
} // love
