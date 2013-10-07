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

#include "common/config.h"
#include "SpriteBatch.h"

// OpenGL
#include "OpenGL.h"

// LOVE
#include "Image.h"
#include "VertexBuffer.h"

// C++
#include <algorithm>

// C
#include <stddef.h>

namespace love
{
namespace graphics
{
namespace opengl
{

SpriteBatch::SpriteBatch(Image *image, int size, int usage)
	: image(image)
	, size(size)
	, next(0)
	, color(0)
	, array_buf(0)
	, element_buf(0)
{
	if (size <= 0)
		throw love::Exception("Invalid SpriteBatch size.");

	GLenum gl_usage;
	switch (usage)
	{
	default:
	case USAGE_DYNAMIC:
		gl_usage = GL_DYNAMIC_DRAW;
		break;
	case USAGE_STATIC:
		gl_usage = GL_STATIC_DRAW;
		break;
	case USAGE_STREAM:
		gl_usage = GL_STREAM_DRAW;
		break;
	}

	const size_t vertex_size = sizeof(Vertex) * 4 * size;

	try
	{
		array_buf = VertexBuffer::Create(vertex_size, GL_ARRAY_BUFFER, gl_usage);
		element_buf = new VertexIndex(size);
	}
	catch (love::Exception &)
	{
		delete array_buf;
		delete element_buf;
		throw;
	}
	catch (std::bad_alloc &)
	{
		delete array_buf;
		delete element_buf;
		throw love::Exception("Out of memory.");
	}

	image->retain();
}

SpriteBatch::~SpriteBatch()
{
	image->release();

	delete color;
	delete array_buf;
	delete element_buf;
}

int SpriteBatch::add(float x, float y, float a, float sx, float sy, float ox, float oy, float kx, float ky, int index /*= -1*/)
{
	// Only do this if there's a free slot.
	if ((index == -1 && next >= size) || index < -1 || index >= size)
		return -1;

	// Needed for colors.
	memcpy(sprite, image->getVertices(), sizeof(Vertex)*4);

	// Transform.
	static Matrix t;
	t.setTransformation(x, y, a, sx, sy, ox, oy, kx, ky);
	t.transform(sprite, sprite, 4);

	if (color)
		setColorv(sprite, *color);

	addv(sprite, (index == -1) ? next : index);

	// Increment counter.
	if (index == -1)
		return next++;

	return index;
}

int SpriteBatch::addq(Quad *quad, float x, float y, float a, float sx, float sy, float ox, float oy, float kx, float ky, int index /*= -1*/)
{
	// Only do this if there's a free slot.
	if ((index == -1 && next >= size) || index < -1 || index >= next)
		return -1;

	// Needed for colors.
	memcpy(sprite, quad->getVertices(), sizeof(Vertex) * 4);

	static Matrix t;
	t.setTransformation(x, y, a, sx, sy, ox, oy, kx, ky);
	t.transform(sprite, sprite, 4);

	if (color)
		setColorv(sprite, *color);

	addv(sprite, (index == -1) ? next : index);

	// Increment counter.
	if (index == -1)
		return next++;

	return index;
}

void SpriteBatch::clear()
{
	// Reset the position of the next index.
	next = 0;
}

void *SpriteBatch::lock()
{
	VertexBuffer::Bind bind(*array_buf);

	return array_buf->map();
}

void SpriteBatch::unlock()
{
	VertexBuffer::Bind bind(*array_buf);

	array_buf->unmap();
}

void SpriteBatch::setImage(Image *newimage)
{
	Object::AutoRelease imagerelease(image);

	newimage->retain();
	image = newimage;
}

Image *SpriteBatch::getImage()
{
	return image;
}

void SpriteBatch::setColor(const Color &color)
{
	if (!this->color)
		this->color = new Color(color);
	else
		*(this->color) = color;
}

void SpriteBatch::setColor()
{
	delete color;
	color = 0;
}

const Color *SpriteBatch::getColor() const
{
	return color;
}

int SpriteBatch::getCount() const
{
	return next;
}

void SpriteBatch::setBufferSize(int newsize)
{
	if (newsize <= 0)
		throw love::Exception("Invalid SpriteBatch size.");

	if (newsize == size)
		return;

	// Map (lock) the old VertexBuffer to get a pointer to its data.
	void *old_data = lock();

	size_t vertex_size = sizeof(Vertex) * 4 * newsize;

	VertexBuffer *new_array_buf = 0;
	VertexIndex *new_element_buf = 0;
	void *new_data = 0;

	try
	{
		new_array_buf = VertexBuffer::Create(vertex_size, array_buf->getTarget(), array_buf->getUsage());
		new_element_buf = new VertexIndex(newsize);

		// VBO::map can throw an exception. Also we want to scope the bind.
		VertexBuffer::Bind bind(*new_array_buf);
		new_data = new_array_buf->map();
	}
	catch (love::Exception &)
	{
		delete new_array_buf;
		delete new_element_buf;
		unlock();
		throw;
	}

	// Copy as much of the old data into the new VertexBuffer as can fit.
	memcpy(new_data, old_data, sizeof(Vertex) * 4 * std::min(newsize, size));

	// We don't need to unmap the old VertexBuffer since we're deleting it.
	delete array_buf;
	delete element_buf;

	array_buf = new_array_buf;
	element_buf = new_element_buf;
	size = newsize;

	next = std::min(next, newsize);

	// But we should unmap (unlock) the new one!
	unlock();
}

int SpriteBatch::getBufferSize() const
{
	return size;
}

void SpriteBatch::draw(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky) const
{
	const size_t vertex_offset = offsetof(Vertex, x);
	const size_t texel_offset = offsetof(Vertex, s);
	const size_t color_offset = offsetof(Vertex, r);

	if (next == 0)
		return;

	static Matrix t;

	glPushMatrix();

	t.setTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);
	glMultMatrixf((const GLfloat *)t.getElements());

	image->predraw();

	VertexBuffer::Bind array_bind(*array_buf);
	VertexBuffer::Bind element_bind(*element_buf->getVertexBuffer());

	Color curcolor = gl.getColor();

	// Apply per-sprite color, if a color is set.
	if (color)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), array_buf->getPointer(color_offset));
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), array_buf->getPointer(vertex_offset));

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), array_buf->getPointer(texel_offset));

	glDrawElements(GL_TRIANGLES, element_buf->getIndexCount(next), element_buf->getType(), element_buf->getPointer(0));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (color)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		gl.setColor(curcolor);
	}

	image->postdraw();

	glPopMatrix();
}

void SpriteBatch::addv(const Vertex *v, int index)
{
	static const int sprite_size = 4 * sizeof(Vertex); // bytecount
	VertexBuffer::Bind bind(*array_buf);
	array_buf->fill(index * sprite_size, sprite_size, v);
}

void SpriteBatch::setColorv(Vertex *v, const Color &color)
{
	for (size_t i = 0; i < 4; ++i)
	{
		v[i].r = color.r;
		v[i].g = color.g;
		v[i].b = color.b;
		v[i].a = color.a;
	}
}

bool SpriteBatch::getConstant(const char *in, UsageHint &out)
{
	return usageHints.find(in, out);
}

bool SpriteBatch::getConstant(UsageHint in, const char *&out)
{
	return usageHints.find(in, out);
}

StringMap<SpriteBatch::UsageHint, SpriteBatch::USAGE_MAX_ENUM>::Entry SpriteBatch::usageHintEntries[] =
{
	{"dynamic", SpriteBatch::USAGE_DYNAMIC},
	{"static", SpriteBatch::USAGE_STATIC},
	{"stream", SpriteBatch::USAGE_STREAM},
};

StringMap<SpriteBatch::UsageHint, SpriteBatch::USAGE_MAX_ENUM> SpriteBatch::usageHints(usageHintEntries, sizeof(usageHintEntries));

} // opengl
} // graphics
} // love
