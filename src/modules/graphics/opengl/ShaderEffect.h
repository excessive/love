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

	// pointer to currently active ShaderEffect.
	static ShaderEffect *current;
	
	// Only vertex and fragment shaders have guaranteed support in all ShaderEffects.
	enum ShaderType
	{
		TYPE_VERTEX,
		TYPE_TESSCONTROL,
		TYPE_TESSEVAL,
		TYPE_GEOMETRY,
		TYPE_FRAGMENT,
		TYPE_MAX_ENUM
	};

	// type for a list of shader source codes in the form of sources[shadertype] = code
	typedef std::map<ShaderType, std::string> ShaderSources;

	/**
	 * Creates a new ShaderEffect using a list of source codes.
	 * Sources must contain at least one vertex or fragment shader.
	 **/
	ShaderEffect(const ShaderSources &shadersources);

	virtual ~ShaderEffect();

	// Implements Volatile
	virtual bool loadVolatile();
	virtual void unloadVolatile();

	/**
	 * Binds this ShaderEffect's program to be used when rendering.
	 *
	 * @param temporary True if we just want to send values to the shader with no intention of rendering.
	 **/
	void attach(bool temporary = false);

	/**
	 * Detach the currently bound ShaderEffect.
	 * Causes OpenGL to use fixed functionality in place of shader programs.
	 **/
	static void detach();

	/**
	 * Returns any warnings this ShaderEffect may have generated.
	 **/
	std::string getWarnings() const;

	/**
	 * Send at least one float or vector value to this ShaderEffect as a uniform.
	 *
	 * @param name The name of the uniform variable in the source code.
	 * @param size Number of elements in each vector to send.
	 *             A value of 1 indicates a single-component vector (a float).
	 * @param vec Pointer to the float or vector values.
	 * @param count Number of float or vector values.
	 **/
	void sendFloat(const std::string &name, int size, const GLfloat *vec, int count);

	/**
	 * Send at least one matrix to this ShaderEffect as a uniform.
	 *
	 * @param name The name of the uniform variable in the source code.
	 * @param size Number of rows/columns in the matrix.
	 * @param m Pointer to the first element of the first matrix.
	 * @param count Number of matrices to send.
	 **/
	void sendMatrix(const std::string &name, int size, const GLfloat *m, int count);

	/**
	 * Send an image to this ShaderEffect as a uniform.
	 *
	 * @param name The name of the uniform variable in the source code.
	 **/
	void sendImage(const std::string &name, const Image &image);

	/**
	 * Send a canvas to this ShaderEffect as a uniform.
	 *
	 * @param name The name of the uniform variable in the source code.
	 **/
	void sendCanvas(const std::string &name, const Canvas &canvas);

	static std::string getGLSLVersion();
	static bool isSupported();

private:

	GLint getUniformLocation(const std::string &name);
	void checkSetUniformError();
	
	GLuint createShader(ShaderType type, const std::string &code);
	void createProgram(const std::vector<GLuint> &shaderids);

	int getTextureUnit(const std::string &name);

	void sendTexture(const std::string &name, GLuint texture);

	// list of all shader code attached to this ShaderEffect
	ShaderSources _shadersources;

	GLuint _program; // volatile

	// uniform location buffer map
	std::map<std::string, GLint> _uniforms;

	// texture unit pool for setting images
	std::map<std::string, GLint> _textureunitpool; // _textureunitpool[name] = textureunit
	std::vector<GLuint> _activetextureunits; // _activetextureunits[textureunit-1] = textureid

	// max GPU texture units available for sent images
	static GLint _maxtextureunits;

	// counts total number of textures bound to each texture unit in all shaders
	static std::vector<int> _texturecounters;
};

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_EFFECT_H
