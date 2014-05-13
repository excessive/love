/**
 * Copyright (c) 2006-2014 LOVE Development Team
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

#include "Canvas.h"
#include "Image.h"
#include "Graphics.h"
#include "common/Matrix.h"
#include "common/config.h"

#include <cstring> // For memcpy
#include <limits>

namespace love
{
namespace graphics
{
namespace opengl
{

// strategy for fbo creation, interchangable at runtime:
// none, opengl >= 3.0, extensions
struct FramebufferStrategy
{
	virtual ~FramebufferStrategy() {}

	/// create a new framebuffer and texture
	/**
	 * @param[out] framebuffer Framebuffer name
	 * @param[in]  texture     Texture name
	 * @return Creation status
	 */
	virtual GLenum createFBO(GLuint &, GLuint)
	{
		return GL_FRAMEBUFFER_UNSUPPORTED;
	}

	/// Create a stencil buffer and attach it to the active framebuffer object
	/**
	 * @param[in]  width   Width of the stencil buffer
	 * @param[in]  height  Height of the stencil buffer
	 * @param[in]  samples Number of samples to use
	 * @param[out] stencil Name for stencil buffer
	 * @return Whether the stencil buffer was successfully created
	 **/
	virtual bool createStencil(int, int, int, GLuint &)
	{
		return false;
	}

	/// Create a MSAA renderbuffer and attach it to the active FBO.
	/**
	 * @param[in]    width Width of the MSAA buffer
	 * @param[in]    height Height of the MSAA buffer
	 * @param[inout] samples Number of samples to use
	 * @param[in]    internalformat The internal format to use for the buffer
	 * @param[out]   buffer Name for the MSAA buffer
	 * @return Whether the MSAA buffer was successfully created and attached
	 **/
	virtual bool createMSAABuffer(int, int, int &, GLenum, GLuint &)
	{
		return false;
	}

	/// remove objects
	/**
	 * @param[in] framebuffer   Framebuffer name
	 * @param[in] depth_stencil Name for packed depth and stencil buffer
	 */
	virtual void deleteFBO(GLuint, GLuint, GLuint) {}
	virtual void bindFBO(GLuint) {}

	/// attach additional canvases to the active framebuffer for rendering
	/**
	 * @param[in] canvases List of canvases to attach
	 **/
	virtual void setAttachments(const std::vector<Canvas *> &) {}

	/// stop using all additional attached canvases
	virtual void setAttachments() {}
};

struct FramebufferStrategyGL3 : public FramebufferStrategy
{
	virtual GLenum createFBO(GLuint &framebuffer, GLuint texture)
	{
		// get currently bound fbo to reset to it later
		GLint current_fbo;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);

		// create framebuffer
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		if (texture != 0)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                       GL_TEXTURE_2D, texture, 0);
		}

		// check status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		// unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, (GLuint) current_fbo);
		return status;
	}

	virtual bool createStencil(int width, int height, int samples, GLuint &stencil)
	{
		// create combined depth/stencil buffer
		glDeleteRenderbuffers(1, &stencil);
		glGenRenderbuffers(1, &stencil);
		glBindRenderbuffer(GL_RENDERBUFFER, stencil);

		if (samples > 1)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_STENCIL, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		                          GL_RENDERBUFFER, stencil);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// check status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffers(1, &stencil);
			stencil = 0;
			return false;
		}

		return true;
	}

	virtual bool createMSAABuffer(int width, int height, int &samples, GLenum internalformat, GLuint &buffer)
	{
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat,
		                                 width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                          GL_RENDERBUFFER, buffer);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffers(1, &buffer);
			buffer = 0;
			return false;
		}

		return true;
	}

	virtual void deleteFBO(GLuint framebuffer, GLuint depth_stencil, GLuint msaa_buffer)
	{
		if (depth_stencil != 0)
			glDeleteRenderbuffers(1, &depth_stencil);
		if (msaa_buffer != 0)
			glDeleteRenderbuffers(1, &msaa_buffer);
		if (framebuffer != 0)
			glDeleteFramebuffers(1, &framebuffer);
	}

	virtual void bindFBO(GLuint framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	virtual void setAttachments()
	{
		// set a single render target
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}

	virtual void setAttachments(const std::vector<Canvas *> &canvases)
	{
		if (canvases.size() == 0)
		{
			setAttachments();
			return;
		}

		std::vector<GLenum> drawbuffers;
		drawbuffers.push_back(GL_COLOR_ATTACHMENT0);

		// Attach the canvas textures to the currently bound framebuffer.
		for (size_t i = 0; i < canvases.size(); i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i,
				GL_TEXTURE_2D, canvases[i]->getGLTexture(), 0);
			drawbuffers.push_back(GL_COLOR_ATTACHMENT1 + i);
		}

		// set up multiple render targets
		if (GLEE_VERSION_2_0)
			glDrawBuffers(drawbuffers.size(), &drawbuffers[0]);
		else if (GLEE_ARB_draw_buffers)
			glDrawBuffersARB(drawbuffers.size(), &drawbuffers[0]);
	}
};

struct FramebufferStrategyPackedEXT : public FramebufferStrategy
{
	virtual GLenum createFBO(GLuint &framebuffer, GLuint texture)
	{
		GLint current_fbo;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_EXT, &current_fbo);

		// create framebuffer
		glGenFramebuffersEXT(1, &framebuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, texture, 0);

		// check status
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

		// unbind framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint) current_fbo);
		return status;
	}

	virtual bool createStencil(int width, int height, int samples, GLuint &stencil)
	{
		// create combined depth/stencil buffer
		glDeleteRenderbuffersEXT(1, &stencil);
		glGenRenderbuffersEXT(1, &stencil);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencil);

		if (samples > 1)
		{
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, samples,
			                                    GL_DEPTH_STENCIL, width, height);
		}
		else
		{
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT,
			                         width, height);
		}

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
									 GL_RENDERBUFFER_EXT, stencil);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

		// check status
		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffersEXT(1, &stencil);
			stencil = 0;
			return false;
		}

		return true;
	}

	virtual bool createMSAABuffer(int width, int height, int &samples, GLenum internalformat, GLuint &buffer)
	{
		if (!GLEE_EXT_framebuffer_multisample)
			return false;

		glGenRenderbuffersEXT(1, &buffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER, buffer);
		glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, samples,
		                                    internalformat, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                             GL_RENDERBUFFER, buffer);
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);

		glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffersEXT(1, &buffer);
			buffer = 0;
			return false;
		}

		return true;
	}

	virtual void deleteFBO(GLuint framebuffer, GLuint depth_stencil, GLuint msaa_buffer)
	{
		if (depth_stencil != 0)
			glDeleteRenderbuffersEXT(1, &depth_stencil);
		if (msaa_buffer != 0)
			glDeleteRenderbuffersEXT(1, &msaa_buffer);
		if (framebuffer != 0)
			glDeleteFramebuffersEXT(1, &framebuffer);
	}

	virtual void bindFBO(GLuint framebuffer)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	}

	virtual void setAttachments()
	{
		// set a single render target
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	}

	virtual void setAttachments(const std::vector<Canvas *> &canvases)
	{
		if (canvases.size() == 0)
		{
			setAttachments();
			return;
		}

		std::vector<GLenum> drawbuffers;
		drawbuffers.push_back(GL_COLOR_ATTACHMENT0_EXT);

		// Attach the canvas textures to the currently bound framebuffer.
		for (size_t i = 0; i < canvases.size(); i++)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1_EXT + i,
								   GL_TEXTURE_2D, canvases[i]->getGLTexture(), 0);
			drawbuffers.push_back(GL_COLOR_ATTACHMENT1_EXT + i);
		}

		// set up multiple render targets
		if (GLEE_VERSION_2_0)
			glDrawBuffers(drawbuffers.size(), &drawbuffers[0]);
		else if (GLEE_ARB_draw_buffers)
			glDrawBuffersARB(drawbuffers.size(), &drawbuffers[0]);
	}
};

struct FramebufferStrategyEXT : public FramebufferStrategyPackedEXT
{
	virtual bool createStencil(int width, int height, int samples, GLuint &stencil)
	{
		// create stencil buffer
		glDeleteRenderbuffersEXT(1, &stencil);
		glGenRenderbuffersEXT(1, &stencil);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencil);

		if (samples > 1)
		{
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, samples,
			                                    GL_STENCIL_INDEX, width, height);
		}
		else
		{
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX,
			                         width, height);
		}

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
									 GL_RENDERBUFFER_EXT, stencil);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

		// check status
		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffersEXT(1, &stencil);
			stencil = 0;
			return false;
		}

		return true;
	}

	bool isSupported()
	{
		GLuint fb = 0, stencil = 0;
		GLenum status = createFBO(fb, 0);
		deleteFBO(fb, stencil, 0);
		return status == GL_FRAMEBUFFER_COMPLETE;
	}
};

FramebufferStrategy *strategy = nullptr;

FramebufferStrategy strategyNone;

FramebufferStrategyGL3 strategyGL3;

FramebufferStrategyPackedEXT strategyPackedEXT;

FramebufferStrategyEXT strategyEXT;

Canvas *Canvas::current = nullptr;
OpenGL::Viewport Canvas::systemViewport = OpenGL::Viewport();
bool Canvas::screenHasSRGB = false;

static void getStrategy()
{
	if (!strategy)
	{
		if (GLEE_VERSION_3_0 || GLEE_ARB_framebuffer_object)
			strategy = &strategyGL3;
		else if (GLEE_EXT_framebuffer_object && GLEE_EXT_packed_depth_stencil)
			strategy = &strategyPackedEXT;
		else if (GLEE_EXT_framebuffer_object && strategyEXT.isSupported())
			strategy = &strategyEXT;
		else
			strategy = &strategyNone;
	}
}

Canvas::Canvas(int width, int height, Format format, int fsaa)
	: fbo(0)
    , resolve_fbo(0)
	, texture(0)
    , fsaa_buffer(0)
	, depth_stencil(0)
	, format(format)
    , fsaa_samples(fsaa)
	, fsaa_dirty(false)
{
	this->width = width;
	this->height = height;

	float w = static_cast<float>(width);
	float h = static_cast<float>(height);

	// world coordinates
	vertices[0].x = 0;
	vertices[0].y = 0;
	vertices[1].x = 0;
	vertices[1].y = h;
	vertices[2].x = w;
	vertices[2].y = h;
	vertices[3].x = w;
	vertices[3].y = 0;

	// texture coordinates
	vertices[0].s = 0;
	vertices[0].t = 0;
	vertices[1].s = 0;
	vertices[1].t = 1;
	vertices[2].s = 1;
	vertices[2].t = 1;
	vertices[3].s = 1;
	vertices[3].t = 0;

	getStrategy();

	loadVolatile();
}

Canvas::~Canvas()
{
	// reset framebuffer if still using this one
	if (current == this)
		stopGrab();

	unloadVolatile();
}

bool Canvas::createFSAAFBO(GLenum internalformat)
{
	// Create our FBO without a texture.
	status = strategy->createFBO(fbo, 0);

	GLuint previous = 0;
	if (current != this)
	{
		if (current != nullptr)
			previous = current->fbo;

		strategy->bindFBO(fbo);
	}

	// Create and attach the MSAA buffer for our FBO.
	if (strategy->createMSAABuffer(width, height, fsaa_samples, internalformat, fsaa_buffer))
		status = GL_FRAMEBUFFER_COMPLETE;
	else
		status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

	// Create the FBO used for the MSAA resolve, and attach the texture.
	if (status == GL_FRAMEBUFFER_COMPLETE)
		status = strategy->createFBO(resolve_fbo, texture);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		// Clean up.
		strategy->deleteFBO(fbo, 0, fsaa_buffer);
		strategy->deleteFBO(resolve_fbo, 0, 0);
		fbo = fsaa_buffer = resolve_fbo = 0;
		fsaa_samples = 0;
	}

	if (current != this)
		strategy->bindFBO(previous);

	return status == GL_FRAMEBUFFER_COMPLETE;
}

bool Canvas::loadVolatile()
{
	fbo = depth_stencil = texture = 0;
	resolve_fbo = fsaa_buffer = 0;
	status = GL_FRAMEBUFFER_COMPLETE;

	// glTexImage2D is guaranteed to error in this case.
	if (width > gl.getMaxTextureSize() || height > gl.getMaxTextureSize())
	{
		status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		return false;
	}

	if (!isFormatSupported(format))
	{
		status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		return false;
	}

	glGenTextures(1, &texture);
	gl.bindTexture(texture);

	setFilter(filter);
	setWrap(wrap);

	GLenum internalformat = GL_RGBA;
	GLenum externalformat = GL_RGBA;
	GLenum textype = GL_UNSIGNED_BYTE;

	convertFormat(format, internalformat, externalformat, textype);

	while (glGetError() != GL_NO_ERROR)
		/* Clear the error buffer. */;

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             (GLint) internalformat,
	             width, height,
	             0,
	             externalformat,
	             textype,
	             nullptr);

	if (glGetError() != GL_NO_ERROR)
	{
		status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		return false;
	}

	int max_samples = 0;
	if (GLEE_VERSION_3_0 || GLEE_ARB_framebuffer_object
		|| GLEE_EXT_framebuffer_multisample)
	{
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	}

	if (fsaa_samples > max_samples)
		fsaa_samples = max_samples;

	// Try to create a FSAA FBO if requested.
	bool fsaasuccess = false;
	if (fsaa_samples > 1)
		fsaasuccess = createFSAAFBO(internalformat);

	// On failure (or no requested FSAA), fall back to a regular FBO.
	if (!fsaasuccess)
		status = strategy->createFBO(fbo, texture);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	clear(Color(0, 0, 0, 0));

	fsaa_dirty = (fsaa_buffer != 0);

	return true;
}

void Canvas::unloadVolatile()
{
	strategy->deleteFBO(fbo, depth_stencil, fsaa_buffer);
	strategy->deleteFBO(resolve_fbo, 0, 0);

	gl.deleteTexture(texture);

	fbo = depth_stencil = texture = 0;
	resolve_fbo = fsaa_buffer = 0;

	for (size_t i = 0; i < attachedCanvases.size(); i++)
		attachedCanvases[i]->release();

	attachedCanvases.clear();
}

void Canvas::drawv(const Matrix &t, const Vertex *v)
{
	glPushMatrix();
	glMultMatrixf((const GLfloat *)t.getElements());

	predraw();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid *)&v[0].x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid *)&v[0].s);

	gl.prepareDraw();
	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	postdraw();
	
	glPopMatrix();
}

void Canvas::draw(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky)
{
	static Matrix t;
	t.setTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);

	drawv(t, vertices);
}

void Canvas::drawq(Quad *quad, float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky)
{
	static Matrix t;
	t.setTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);

	const Vertex *v = quad->getVertices();
	drawv(t, v);
}

void Canvas::setFilter(const Texture::Filter &f)
{
	filter = f;
	gl.bindTexture(texture);
	gl.setTextureFilter(filter);
}

void Canvas::setWrap(const Texture::Wrap &w)
{
	wrap = w;
	gl.bindTexture(texture);
	gl.setTextureWrap(wrap);
}

GLuint Canvas::getGLTexture() const
{
	return texture;
}

void Canvas::predraw()
{
	// We need to make sure the texture is up-to-date by resolving the MSAA
	// buffer (which we render to when the canvas is active) to it.
	resolveMSAA();

	gl.bindTexture(texture);
}

void Canvas::setupGrab()
{
	// already grabbing
	if (current == this)
		return;

	// cleanup after previous Canvas
	if (current != nullptr)
	{
		systemViewport = current->systemViewport;
		current->stopGrab(true);
	}
	else
		systemViewport = gl.getViewport();

	// indicate we are using this Canvas.
	current = this;

	// bind the framebuffer object.
	strategy->bindFBO(fbo);
	gl.setViewport(OpenGL::Viewport(0, 0, width, height));

	// Reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Set up orthographic view (no depth)
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

	// Switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// Make sure the correct sRGB setting is used when drawing to the canvas.
	if (format == FORMAT_SRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);
	else if (screenHasSRGB)
		glDisable(GL_FRAMEBUFFER_SRGB);

	if (fsaa_buffer != 0)
		fsaa_dirty = true;
}

void Canvas::startGrab(const std::vector<Canvas *> &canvases)
{
	// Whether the new canvas list is different from the old one.
	// A more thorough check is done below.
	bool canvaseschanged = canvases.size() != attachedCanvases.size();

	if (canvases.size() > 0)
	{
		if (!isMultiCanvasSupported())
			throw love::Exception("Multi-canvas rendering is not supported on this system.");

		if ((int) canvases.size() + 1 > gl.getMaxRenderTargets())
			throw love::Exception("This system can't simultaniously render to %d canvases.", canvases.size()+1);

		if (fsaa_samples != 0)
			throw love::Exception("Multi-canvas rendering is not supported with FSAA.");
	}

	for (size_t i = 0; i < canvases.size(); i++)
	{
		if (canvases[i]->getWidth() != width || canvases[i]->getHeight() != height)
			throw love::Exception("All canvas arguments must have the same dimensions.");

		if (canvases[i]->getTextureFormat() != format)
			throw love::Exception("All canvas arguments must have the same texture format.");

		if (canvases[i]->getFSAA() != 0)
			throw love::Exception("Multi-canvas rendering is not supported with FSAA.");

		if (!canvaseschanged && canvases[i] != attachedCanvases[i])
			canvaseschanged = true;
	}

	setupGrab();

	// Don't attach anything if there's nothing to change.
	if (!canvaseschanged)
		return;

	// Attach the canvas textures to the active FBO and set up MRTs.
	strategy->setAttachments(canvases);

	for (size_t i = 0; i < canvases.size(); i++)
		canvases[i]->retain();

	for (size_t i = 0; i < attachedCanvases.size(); i++)
		attachedCanvases[i]->release();

	attachedCanvases = canvases;
}

void Canvas::startGrab()
{
	setupGrab();

	if (attachedCanvases.size() == 0)
		return;

	// make sure the FBO is only using a single canvas
	strategy->setAttachments();

	// release any previously attached canvases
	for (size_t i = 0; i < attachedCanvases.size(); i++)
		attachedCanvases[i]->release();

	attachedCanvases.clear();
}

void Canvas::stopGrab(bool switchingToOtherCanvas)
{
	// i am not grabbing. leave me alone
	if (current != this)
		return;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (switchingToOtherCanvas)
	{
		if (format == FORMAT_SRGB)
			glDisable(GL_FRAMEBUFFER_SRGB);
	}
	else
	{
		// bind system framebuffer.
		strategy->bindFBO(0);
		current = nullptr;
		gl.setViewport(systemViewport);

		if (format == FORMAT_SRGB && !screenHasSRGB)
			glDisable(GL_FRAMEBUFFER_SRGB);
		else if (format != FORMAT_SRGB && screenHasSRGB)
			glEnable(GL_FRAMEBUFFER_SRGB);
	}
}

void Canvas::clear(Color c)
{
	if (strategy == &strategyNone)
		return;

	GLuint previous = 0;

	if (current != this)
	{
		if (current != nullptr)
			previous = current->fbo;

		strategy->bindFBO(fbo);
	}

	GLfloat glcolor[] = {c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f};

	// We don't need to worry about multiple FBO attachments or global clear
	// color state when OpenGL 3.0+ is supported.
	if (GLEE_VERSION_3_0)
	{
		glClearBufferfv(GL_COLOR, 0, glcolor);

		if (depth_stencil != 0)
		{
			GLint stencilvalue = 0;
			glClearBufferiv(GL_STENCIL, 0, &stencilvalue);
		}
	}
	else
	{
		// glClear will clear all active draw buffers, so we need to temporarily
		// detach any other canvases (when MRT is being used.)
		if (attachedCanvases.size() > 0)
			strategy->setAttachments();

		// Don't use the state-shadowed gl.setClearColor because we want to save
		// the previous clear color.
		glClearColor(glcolor[0], glcolor[1], glcolor[2], glcolor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		if (attachedCanvases.size() > 0)
			strategy->setAttachments(attachedCanvases);

		// Restore the global clear color.
		gl.setClearColor(gl.getClearColor());
	}

	if (current != this)
		strategy->bindFBO(previous);

	if (fsaa_buffer != 0)
		fsaa_dirty = true;
}

bool Canvas::checkCreateStencil()
{
	// Do nothing if we've already created the stencil buffer.
	if (depth_stencil != 0)
		return true;

	if (current != this)
		strategy->bindFBO(fbo);

	bool success = strategy->createStencil(width, height, fsaa_samples, depth_stencil);

	if (current && current != this)
		strategy->bindFBO(current->fbo);
	else if (!current)
		strategy->bindFBO(0);

	return success;
}

love::image::ImageData *Canvas::getImageData(love::image::Image *image)
{
	resolveMSAA();

	int row = 4 * width;
	int size = row * height;
	GLubyte *pixels  = new GLubyte[size];

	// Our texture is attached to 'resolve_fbo' when we use MSAA.
	if (fsaa_samples > 1 && (GLEE_VERSION_3_0 || GLEE_ARB_framebuffer_object))
		glBindFramebuffer(GL_READ_FRAMEBUFFER, resolve_fbo);
	else if (fsaa_samples > 1 && GLEE_EXT_framebuffer_multisample)
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, resolve_fbo);
	else
		strategy->bindFBO(fbo);

	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	if (current)
		strategy->bindFBO(current->fbo);
	else
		strategy->bindFBO(0);

	// The new ImageData now owns the pixel data, so we don't delete it here.
	love::image::ImageData *img = image->newImageData(width, height, (void *)pixels, true);
	return img;
}

void Canvas::getPixel(unsigned char* pixel_rgba, int x, int y)
{
	resolveMSAA();

	// Our texture is attached to 'resolve_fbo' when we use MSAA.
	if (fsaa_samples > 1 && (GLEE_VERSION_3_0 || GLEE_ARB_framebuffer_object))
		glBindFramebuffer(GL_READ_FRAMEBUFFER, resolve_fbo);
	else if (fsaa_samples > 1 && GLEE_EXT_framebuffer_multisample)
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, resolve_fbo);
	else if (current != this)
		strategy->bindFBO(fbo);

	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel_rgba);

	if (current && current != this)
		strategy->bindFBO(current->fbo);
	else if (!current)
		strategy->bindFBO(0);
}

bool Canvas::resolveMSAA()
{
	if (resolve_fbo == 0 || fsaa_buffer == 0)
		return false;

	if (!fsaa_dirty)
		return true;

	GLuint previous = 0;
	if (current != nullptr)
		previous = current->fbo;

	// Do the MSAA resolve by blitting the MSAA renderbuffer to the texture.
	if (GLEE_VERSION_3_0 || GLEE_ARB_framebuffer_object)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolve_fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	else if (GLEE_EXT_framebuffer_multisample && GLEE_EXT_framebuffer_blit)
	{
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, resolve_fbo);
		glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height,
							 GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	else
		return false;

	strategy->bindFBO(previous);

	if (current != this)
		fsaa_dirty = false;

	return true;
}

Canvas::Format Canvas::getSizedFormat(Canvas::Format format)
{
	switch (format)
	{
	case FORMAT_NORMAL:
		return FORMAT_RGBA8;
	case FORMAT_HDR:
		return FORMAT_RGBA16F;
	default:
		return format;
	}
}

void Canvas::convertFormat(Canvas::Format format, GLenum &internalformat, GLenum &externalformat, GLenum &type)
{
	format = getSizedFormat(format);
	externalformat = GL_RGBA;

	switch (format)
	{
	case FORMAT_RGBA8:
	default:
		internalformat = GL_RGBA8;
		type = GL_UNSIGNED_BYTE;
		break;
	case FORMAT_RGBA4:
		internalformat = GL_RGBA4;
		type = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
	case FORMAT_RGB5A1:
		internalformat = GL_RGB5_A1;
		type = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case FORMAT_RGB565:
		internalformat = GL_RGB565;
		externalformat = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case FORMAT_RGB10A2:
		internalformat = GL_RGB10_A2;
		type = GL_UNSIGNED_INT_10_10_10_2;
		break;
	case FORMAT_RGB9E5:
		internalformat = GL_RGB9_E5;
		externalformat = GL_RGB;
		type = GL_RGB9_E5;
	case FORMAT_RG11B10F:
		internalformat = GL_R11F_G11F_B10F;
		externalformat = GL_RGB;
		type = GL_UNSIGNED_INT_10F_11F_11F_REV;
	case FORMAT_RGBA16F:
		internalformat = GL_RGBA16F;
		type = GL_FLOAT;
		break;
	case FORMAT_RGBA32F:
		internalformat = GL_RGBA32F;
		type = GL_FLOAT;
		break;
	case FORMAT_SRGB:
		internalformat = GL_SRGB8_ALPHA8;
		type = GL_UNSIGNED_BYTE;
		break;
	}
}

bool Canvas::isSupported()
{
	getStrategy();
	return (strategy != &strategyNone);
}

bool Canvas::isMultiCanvasSupported()
{
	// system must support at least 4 simultanious active canvases.
	return gl.getMaxRenderTargets() >= 4;
}

bool Canvas::isFormatSupported(Canvas::Format format)
{
	if (!isSupported())
		return false;

	format = getSizedFormat(format);

	switch (format)
	{
	case FORMAT_RGBA8:
	case FORMAT_RGBA4:
	case FORMAT_RGB5A1:
	case FORMAT_RGB10A2:
		return true;
	case FORMAT_RGB565:
		return GLEE_VERSION_4_2 || GLEE_ARB_ES2_compatibility;
	case FORMAT_RGB9E5:
		return GLEE_VERSION_3_0 || GLEE_EXT_texture_shared_exponent;
	case FORMAT_RG11B10F:
		return GLEE_VERSION_3_0 || (GLEE_ARB_texture_float && GLEE_ARB_color_buffer_float
			&& GLEE_EXT_packed_float);
	case FORMAT_RGBA16F:
	case FORMAT_RGBA32F:
		return GLEE_VERSION_3_0 || (GLEE_ARB_texture_float && GLEE_ARB_color_buffer_float);
	case FORMAT_SRGB:
		return GLEE_VERSION_3_0 || ((GLEE_ARB_framebuffer_sRGB || GLEE_EXT_framebuffer_sRGB)
			&& (GLEE_VERSION_2_1 || GLEE_EXT_texture_sRGB));
	default:
		return false;
	}
}

void Canvas::bindDefaultCanvas()
{
	if (current != nullptr)
		current->stopGrab();
}

bool Canvas::getConstant(const char *in, Format &out)
{
	return formats.find(in, out);
}

bool Canvas::getConstant(Format in, const char *&out)
{
	return formats.find(in, out);
}

StringMap<Canvas::Format, Canvas::FORMAT_MAX_ENUM>::Entry Canvas::formatEntries[] =
{
	{"normal", Canvas::FORMAT_NORMAL},
	{"hdr", Canvas::FORMAT_HDR},
	{"rgba8", Canvas::FORMAT_RGBA8},
	{"rgba4", Canvas::FORMAT_RGBA4},
	{"rgb5a1", Canvas::FORMAT_RGB5A1},
	{"rgb565", Canvas::FORMAT_RGB565},
	{"rgb10a2", Canvas::FORMAT_RGB10A2},
	{"rgb9e5", Canvas::FORMAT_RGB9E5},
	{"rg11b10f", Canvas::FORMAT_RG11B10F},
	{"rgba16f", Canvas::FORMAT_RGBA16F},
	{"rgba32f", Canvas::FORMAT_RGBA32F},
	{"srgb", Canvas::FORMAT_SRGB},
};

StringMap<Canvas::Format, Canvas::FORMAT_MAX_ENUM> Canvas::formats(Canvas::formatEntries, sizeof(Canvas::formatEntries));

} // opengl
} // graphics
} // love
