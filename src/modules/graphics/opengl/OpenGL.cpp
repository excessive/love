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

#include "OpenGL.h"

namespace love
{
namespace graphics
{
namespace opengl
{

static GLuint boundTexture = 0;

void resetBoundTexture()
{
	// OpenGL might not be initialized yet, so we can't do a real reset
	boundTexture = 0;
}

void bindTexture(GLuint texture, bool override)
{
	if (texture != boundTexture || texture == 0 || override)
	{
		boundTexture = texture;
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}

void deleteTexture(GLuint texture)
{
	if (texture == boundTexture)
		boundTexture = 0;

	glDeleteTextures(1, &texture);
}

} // opengl
} // graphics
} // love
