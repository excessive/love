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

#include <vector>
#include "OpenGL.h"
#include "common/Exception.h"

namespace love
{
namespace graphics
{
namespace opengl
{

static bool contextInitialized = false;

static int curTextureUnitIndex = 0;
static std::vector<GLuint> textureUnits;

void initializeContext()
{
	if (contextInitialized)
		return;
	
	contextInitialized = true;
	
	if (GLEE_ARB_multitexture || GLEE_VERSION_1_3)
	{
		GLint maxtextureunits;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxtextureunits);
		
		if (GLEE_VERSION_2_0 || GLEE_ARB_vertex_shader)
		{
			GLint maxtextureimageunits;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxtextureimageunits);
			
			if (maxtextureimageunits > maxtextureunits)
				maxtextureunits = maxtextureimageunits;
		}
		
		textureUnits.resize(maxtextureunits, 0);
		
		GLenum activeTextureUnit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&activeTextureUnit);
		
		curTextureUnitIndex = activeTextureUnit - GL_TEXTURE0;
	}
	else
	{
		// multitexturing not supported so we only have 1 texture unit
		textureUnits.resize(1, 0);
		curTextureUnitIndex = 0;
	}
}

void uninitializeContext()
{
	contextInitialized = false;
}

void setActiveTextureUnit(GLenum textureunit)
{
	initializeContext();
	
	int textureunitindex = textureunit - GL_TEXTURE0;
	
	if (textureunitindex < 0 || (size_t) textureunitindex >= textureUnits.size())
		throw love::Exception("Invalid texture unit index.");
	
	if (textureunitindex != curTextureUnitIndex)
	{
		if (GLEE_VERSION_1_3)
			glActiveTexture(textureunit);
		else if (GLEE_ARB_multitexture)
			glActiveTextureARB(textureunit);
		else
			throw love::Exception("Multitexturing not supported.");
	}
	
	curTextureUnitIndex = textureunitindex;
}

void bindTexture(GLuint texture)
{
	initializeContext();
	
	if (texture != textureUnits[curTextureUnitIndex] || texture == 0)
	{
		textureUnits[curTextureUnitIndex] = texture;
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}

void deleteTexture(GLuint texture)
{
	initializeContext();
	
	std::vector<GLuint>::iterator it;
	for (it = textureUnits.begin(); it != textureUnits.end(); ++it)
	{
		if (*it == texture)
			*it = 0;
	}

	glDeleteTextures(1, &texture);
}

} // opengl
} // graphics
} // love
