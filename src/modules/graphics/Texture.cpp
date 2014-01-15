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

#include "Texture.h"

namespace love
{
namespace graphics
{

Texture::Filter Texture::defaultFilter;

Texture::Filter::Filter()
	: min(FILTER_LINEAR)
	, mag(FILTER_LINEAR)
	, mipmap(FILTER_NONE)
	, anisotropy(1.0f)
{
}

Texture::Wrap::Wrap()
	: s(WRAP_CLAMP)
	, t(WRAP_CLAMP)
{
}

Texture::Texture()
	: width(0)
	, height(0)
	, filter(getDefaultFilter())
	, wrap()
	, vertices()
{
}

Texture::~Texture()
{
}

int Texture::getWidth() const
{
	return width;
}

int Texture::getHeight() const
{
	return height;
}

const Texture::Filter &Texture::getFilter() const
{
	return filter;
}

const Texture::Wrap &Texture::getWrap() const
{
	return wrap;
}

const Vertex *Texture::getVertices() const
{
	return vertices;
}

void Texture::setDefaultFilter(const Filter &f)
{
	defaultFilter = f;
}

const Texture::Filter &Texture::getDefaultFilter()
{
	return defaultFilter;
}

bool Texture::getConstant(const char *in, FilterMode &out)
{
	return filterModes.find(in, out);
}

bool Texture::getConstant(FilterMode in, const char  *&out)
{
	return filterModes.find(in, out);
}

bool Texture::getConstant(const char *in, WrapMode &out)
{
	return wrapModes.find(in, out);
}

bool Texture::getConstant(WrapMode in, const char  *&out)
{
	return wrapModes.find(in, out);
}

StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM>::Entry Texture::filterModeEntries[] =
{
	{ "linear", Texture::FILTER_LINEAR },
	{ "nearest", Texture::FILTER_NEAREST },
};

StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM> Texture::filterModes(Texture::filterModeEntries, sizeof(Texture::filterModeEntries));

StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM>::Entry Texture::wrapModeEntries[] =
{
	{ "clamp", Texture::WRAP_CLAMP },
	{ "repeat", Texture::WRAP_REPEAT },
};

StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM> Texture::wrapModes(Texture::wrapModeEntries, sizeof(Texture::wrapModeEntries));


} // graphics
} // love
