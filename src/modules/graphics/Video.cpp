/**
 * Copyright (c) 2006-2017 LOVE Development Team
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

#include "Video.h"

// LOVE
#include "Shader.h"
#include "Graphics.h"

namespace love
{
namespace graphics
{

love::Type Video::type("Video", &Drawable::type);

Video::Video(love::video::VideoStream *stream, float pixeldensity)
	: stream(stream)
	, width(stream->getWidth() / pixeldensity)
	, height(stream->getHeight() / pixeldensity)
	, filter(Texture::defaultFilter)
{
	textureHandles[2] = textureHandles[1] = textureHandles[0] = 0;

	filter.mipmap = Texture::FILTER_NONE;

	stream->fillBackBuffer();

	for (int i = 0; i < 4; i++)
		vertices[i].color = Color(255, 255, 255, 255);

	// Vertices are ordered for use with triangle strips:
	// 0---2
	// | / |
	// 1---3
	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;
	vertices[1].x = 0.0f;
	vertices[1].y = (float) height;
	vertices[2].x = (float) width;
	vertices[2].y = 0.0f;
	vertices[3].x = (float) width;
	vertices[3].y = (float) height;

	vertices[0].s = 0.0f;
	vertices[0].t = 0.0f;
	vertices[1].s = 0.0f;
	vertices[1].t = 1.0f;
	vertices[2].s = 1.0f;
	vertices[2].t = 0.0f;
	vertices[3].s = 1.0f;
	vertices[3].t = 1.0f;
}

Video::~Video()
{
}

love::video::VideoStream *Video::getStream()
{
	return stream;
}

void Video::draw(Graphics *gfx, const Matrix4 &m)
{
	update();

	gfx->flushStreamDraws();

	love::graphics::Shader *shader = Shader::current;
	bool usingdefaultshader = (shader == Shader::defaultShader);
	if (usingdefaultshader)
	{
		// If we're using the default shader, substitute the video version.
		Shader::defaultVideoShader->attach();
		shader = Shader::defaultVideoShader;
	}

	shader->setVideoTextures(textureHandles[0], textureHandles[1], textureHandles[2]);

	Graphics::StreamDrawRequest req;
	req.formats[0] = vertex::CommonFormat::XYf_STf_RGBAub;
	req.indexMode = vertex::TriangleIndexMode::QUADS;
	req.vertexCount = 4;

	Graphics::StreamVertexData data = gfx->requestStreamDraw(req);
	Vertex *verts = (Vertex *) data.stream[0];

	Matrix4 t(gfx->getTransform(), m);
	t.transform(verts, vertices, 4);

	Color c = toColor(gfx->getColor());

	for (int i = 0; i < 4; i++)
	{
		verts[i].s = vertices[i].s;
		verts[i].t = vertices[i].t;
		verts[i].color = c;
	}

	gfx->flushStreamDraws();

	if (usingdefaultshader)
		Shader::defaultShader->attach();
}

void Video::update()
{
	bool bufferschanged = stream->swapBuffers();
	stream->fillBackBuffer();

	if (bufferschanged)
	{
		auto frame = (const love::video::VideoStream::Frame*) stream->getFrontBuffer();
		uploadFrame(frame);
	}
}

love::audio::Source *Video::getSource()
{
	return source;
}

void Video::setSource(love::audio::Source *source)
{
	this->source = source;
}

int Video::getWidth() const
{
	return width;
}

int Video::getHeight() const
{
	return height;
}

int Video::getPixelWidth() const
{
	return stream->getWidth();
}

int Video::getPixelHeight() const
{
	return stream->getHeight();
}

const Texture::Filter &Video::getFilter() const
{
	return filter;
}

} // graphics
} // love
