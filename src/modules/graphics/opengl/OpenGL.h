/**
 * Copyright (c) 2006-2012 LOVE Development Team
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

#ifndef LOVE_GRAPHICS_OPENGL_OPENGL_H
#define LOVE_GRAPHICS_OPENGL_OPENGL_H

#include "GLee.h"

namespace love
{
namespace graphics
{
namespace opengl
{

void initializeContext();

void uninitializeContext();

/**
 * Helper for setting the active texture unit
 * @param textureunit The GL texture unit to set
 **/
void setActiveTextureUnit(GLenum textureunit);

/**
 * Helper for binding an OpenGL texture.
 * Makes sure we aren't redundantly binding textures.
 * @param texture The texture to bind.
 **/
void bindTexture(GLuint texture);

/**
 * Helper for binding a texture to a specific texture unit
 * @param texture The texture to bind
 * @param textureunit The texture unit to switch to
 * @param resoreprev Restore previous texture unit when done
 **/
void bindTextureToUnit(GLuint texture, GLenum textureunit, bool restoreprev);

/**
 * Helper for deleting an OpenGL texture.
 * Cleans up if the texture is currently bound.
 * @param texture The texture to delete.
 **/
void deleteTexture(GLuint texture);

} // opengl
} // graphics
} // love

#endif
