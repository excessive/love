/**
* Copyright (c) 2006-2009 LOVE Development Team
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
#include <common/config.h>
#include <common/version.h>
#include <common/runtime.h>
#include <common/MemoryData.h>

#ifdef LOVE_BUILD_EXE

// SDL
#include <SDL.h>

// Modules
#include <audio/wrap_Audio.h>
#include <event/sdl/wrap_Event.h>
#include <filesystem/physfs/wrap_Filesystem.h>
#include <font/freetype/wrap_Font.h>
#include <graphics/opengl/wrap_Graphics.h>
#include <image/wrap_Image.h>
#include <joystick/sdl/wrap_Joystick.h>
#include <keyboard/sdl/wrap_Keyboard.h>
#include <mouse/sdl/wrap_Mouse.h>
#include <native/tcc/wrap_Native.h>
#include <physics/box2d/wrap_Physics.h>
#include <signal/posix/wrap_Signal.h>
#include <sound/wrap_Sound.h>
#include <timer/sdl/wrap_Timer.h>

// Libraries.
#include "libraries/luasocket/luasocket.h"
#include "libraries/lanes/lanes.h"

#endif // LOVE_BUILD_EXE

// Resources
#include "resources/resources.h"

#ifdef LOVE_BUILD_STANDALONE

static const luaL_Reg modules[] = {
	{ "love.audio", love::audio::luaopen_love_audio },
	{ "love.event", love::event::sdl::luaopen_love_event },
	{ "love.filesystem", love::filesystem::physfs::luaopen_love_filesystem },
	{ "love.font", love::font::freetype::luaopen_love_font },
	{ "love.graphics", love::graphics::opengl::luaopen_love_graphics },
	{ "love.image", love::image::luaopen_love_image },
	{ "love.joystick", love::joystick::sdl::luaopen_love_joystick },
	{ "love.keyboard", love::keyboard::sdl::luaopen_love_keyboard },
	{ "love.mouse", love::mouse::sdl::luaopen_love_mouse },
	{ "love.native", love::native::tcc::luaopen_love_native },
	{ "love.physics", love::physics::box2d::luaopen_love_physics },
	{ "love.signal", love::signal::posix::luaopen_love_signal },
	{ "love.sound", love::sound::luaopen_love_sound },
	{ "love.timer", love::timer::sdl::luaopen_love_timer },
	{ 0, 0 }
};

#endif // LOVE_BUILD_STANDALONE

extern "C" LOVE_EXPORT int luaopen_love(lua_State * L)
{
	love::luax_insistglobal(L, "love");

	// Set version information.
	lua_pushinteger(L, love::VERSION);
	lua_setfield(L, -2, "_version");

	lua_pushstring(L, love::VERSION_STR);
	lua_setfield(L, -2, "_version_string");

	lua_pushstring(L, love::VERSION_CODENAME);
	lua_setfield(L, -2, "_version_codename");

	lua_newtable(L);

	for(int i = 0; love::VERSION_COMPATIBILITY[i] != 0; ++i)
	{
		lua_pushinteger(L, love::VERSION_COMPATIBILITY[i]);
		lua_rawseti(L, -2, i+1);
	}

	lua_setfield(L, -2, "_version_compat");


	// Resources.
	for(const love::Resource * r = love::resources; r->name != 0; r++)
	{
		love::luax_newtype(L, "Data", love::DATA_T, new love::MemoryData((void*)r->data, r->size));
		lua_setfield(L, -2, r->name);
	}

	lua_pop(L, 1); // love

#ifdef LOVE_BUILD_STANDALONE

	// Preload module loaders.
	for(int i = 0; modules[i].name != 0; i++)
	{
		love::luax_preload(L, modules[i].func, modules[i].name);
	}

	love::luasocket::__open(L);
	love::lanes::open(L);

#endif // LOVE_BUILD_STANDALONE

	return 0;
}

#ifdef LOVE_BUILD_EXE

int main(int argc, char ** argv)
{
	// Oh, you just want the version? Okay!
	if(argc > 1 && strcmp(argv[1],"--version") == 0) {
		printf("This is LOVE %s (%s), the unquestionably awesome 2D game engine.\n", love::VERSION_STR, love::VERSION_CODENAME);
		return 0;
	}
	// Create the virtual machine.
	lua_State * L = lua_open();
	luaL_openlibs(L);

	love::luax_preload(L, luaopen_love, "love");

	luaopen_love(L);	

	// Add command line arguments to global arg (like stand-alone Lua).
	{
		lua_newtable(L);
		for(int i = 0;i<argc;i++)
		{
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i);
		}
		lua_setglobal(L, "arg");
	}

	// Add love.__exe = true.
	// This indicates that we're running the 
	// standalone version of love, and not the
	// DLL version.
	{
		lua_getglobal(L, "love");
		lua_pushboolean(L, 1);
		lua_setfield(L, -2, "_exe");
		lua_pop(L, 1);
	}

	// This is where we should run the built-in Lua code
	// which gets everything started.

#	include "scripts/boot.lua.h"

	lua_close(L);

#if defined(LOVE_DEBUG) && defined(LOVE_WINDOWS)
	printf("(press key)\n");
	getchar();
#endif
	printf("Done. This was: %s (%s)\n", love::VERSION_STR, love::VERSION_CODENAME);
	return 0;
}

#endif // LOVE_BUILD_EXE
