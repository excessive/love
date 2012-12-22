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

#ifndef LOVE_GRAPHICS_EFFECT_H
#define LOVE_GRAPHICS_EFFECT_H

#include "common/Object.h"
#include <string>
#include <map>
#include <vector>
#include "OpenGL.h"
#include "Image.h"
#include "Canvas.h"

namespace love
{
namespace graphics
{
namespace opengl
{
// A GLSL shader
class ShaderEffect : public Object, public Volatile
{
public:
	ShaderEffect(const std::string &vertcode, const std::string &fragcode);
	virtual ~ShaderEffect();
	std::string getWarnings() const;

	virtual bool loadVolatile();
	virtual void unloadVolatile();

	void attach(bool temporary = false);
	static void detach();
	
	static std::string getGLSLVersion();
	static bool isSupported();

	static ShaderEffect *current;

	void sendFloat(const std::string &name, int size, const GLfloat *vec, int count);
	void sendMatrix(const std::string &name, int size, const GLfloat *m, int count);
	void sendImage(const std::string &name, const Image &image);
	void sendCanvas(const std::string &name, const Canvas &canvas);

private:
	GLint getUniformLocation(const std::string &name);
	void checkSetUniformError();
	GLuint createShader(GLenum type, const std::string &code);
	void createProgram(const std::vector<GLuint> &shaders);
	
	GLuint _program;
	std::string _vertcode;
	std::string _fragcode; // volatile and stuff

	// uniform location buffer
	std::map<std::string, GLint> _uniforms;

	// texture unit pool for setting images
	static GLint _current_texture_unit;
	static GLint _max_texture_units;
	
	std::map<std::string, GLint> _texture_unit_pool;
	std::vector<GLuint> _texture_id_list;
	GLint getTextureUnit(const std::string &name);
	
	void sendTexture(const std::string &name, GLuint texture);
};

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_EFFECT_H
