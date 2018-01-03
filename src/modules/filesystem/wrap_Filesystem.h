/**
 * Copyright (c) 2006-2018 LOVE Development Team
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

#ifndef LOVE_FILESYSTEM_WRAP_FILESYSTEM_H
#define LOVE_FILESYSTEM_WRAP_FILESYSTEM_H

// LOVE
#include "common/runtime.h"
#include "File.h"
#include "FileData.h"

namespace love
{
namespace filesystem
{

/**
 * Gets FileData at the specified index. If the index contains a filepath or
 * a File object, the FileData will be created from that.
 * Note that this function retains the FileData object (possibly by creating it),
 * so a matching release() is required!
 * May trigger a Lua error.
 **/
FileData *luax_getfiledata(lua_State *L, int idx);
bool luax_cangetfiledata(lua_State *L, int idx);
File *luax_getfile(lua_State *L, int idx);

Data *luax_getdata(lua_State *L, int idx);
bool luax_cangetdata(lua_State *L, int idx);

bool hack_setupWriteDirectory();
int loader(lua_State *L);
int extloader(lua_State *L);
extern "C" LOVE_EXPORT int luaopen_love_filesystem(lua_State *L);

} // filesystem
} // love

#endif // LOVE_FILESYSTEM_WRAP_FILESYSTEM_H
