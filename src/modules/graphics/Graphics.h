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

#ifndef LOVE_GRAPHICS_GRAPHICS_H
#define LOVE_GRAPHICS_GRAPHICS_H

// LOVE
#include "common/Module.h"
#include "common/StringMap.h"

namespace love
{
namespace graphics
{

class Graphics : public Module
{
public:

	enum DrawMode
	{
		DRAW_LINE = 1,
		DRAW_FILL,
		DRAW_MAX_ENUM
	};

	enum AlignMode
	{
		ALIGN_LEFT = 1,
		ALIGN_CENTER,
		ALIGN_RIGHT,
		ALIGN_JUSTIFY,
		ALIGN_MAX_ENUM
	};

	enum BlendMode
	{
		BLEND_ALPHA = 1,
		BLEND_ADDITIVE,
		BLEND_SUBTRACTIVE,
		BLEND_MULTIPLICATIVE,
		BLEND_PREMULTIPLIED,
		BLEND_REPLACE,
		BLEND_MAX_ENUM
	};

	enum LineStyle
	{
		LINE_ROUGH = 1,
		LINE_SMOOTH,
		LINE_MAX_ENUM
	};

	enum LineJoin
	{
		LINE_JOIN_NONE = 1,
		LINE_JOIN_MITER,
		LINE_JOIN_BEVEL,
		LINE_JOIN_MAX_ENUM
	};

	enum PointStyle
	{
		POINT_ROUGH = 1,
		POINT_SMOOTH,
		POINT_MAX_ENUM
	};

	enum Support
	{
		SUPPORT_CANVAS = 1,
		SUPPORT_HDR_CANVAS,
		SUPPORT_MULTI_CANVAS,
		SUPPORT_SHADER,
		SUPPORT_NPOT,
		SUPPORT_SUBTRACTIVE,
		SUPPORT_MIPMAP,
		SUPPORT_DXT,
		SUPPORT_BC5,
		SUPPORT_MAX_ENUM
	};

	enum RendererInfo
	{
		RENDERER_INFO_NAME = 1,
		RENDERER_INFO_VERSION,
		RENDERER_INFO_VENDOR,
		RENDERER_INFO_DEVICE,
		RENDERER_INFO_MAX_ENUM
	};

	virtual ~Graphics();

	/**
	 * Sets the current graphics display viewport and initializes the renderer.
	 * @param width The viewport width.
	 * @param height The viewport height.
	 **/
	virtual bool setMode(int width, int height) = 0;

	/**
	 * Un-sets the current graphics display mode (uninitializing objects if
	 * necessary.)
	 **/
	virtual void unSetMode() = 0;

	static bool getConstant(const char *in, DrawMode &out);
	static bool getConstant(DrawMode in, const char  *&out);

	static bool getConstant(const char *in, AlignMode &out);
	static bool getConstant(AlignMode in, const char  *&out);

	static bool getConstant(const char *in, BlendMode &out);
	static bool getConstant(BlendMode in, const char  *&out);

	static bool getConstant(const char *in, LineStyle &out);
	static bool getConstant(LineStyle in, const char  *&out);

	static bool getConstant(const char *in, LineJoin &out);
	static bool getConstant(LineJoin in, const char  *&out);

	static bool getConstant(const char *in, PointStyle &out);
	static bool getConstant(PointStyle in, const char  *&out);

	static bool getConstant(const char *in, Support &out);
	static bool getConstant(Support in, const char  *&out);

	static bool getConstant(const char *in, RendererInfo &out);
	static bool getConstant(RendererInfo in, const char *&out);

private:

	static StringMap<DrawMode, DRAW_MAX_ENUM>::Entry drawModeEntries[];
	static StringMap<DrawMode, DRAW_MAX_ENUM> drawModes;

	static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
	static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;

	static StringMap<BlendMode, BLEND_MAX_ENUM>::Entry blendModeEntries[];
	static StringMap<BlendMode, BLEND_MAX_ENUM> blendModes;

	static StringMap<LineStyle, LINE_MAX_ENUM>::Entry lineStyleEntries[];
	static StringMap<LineStyle, LINE_MAX_ENUM> lineStyles;

	static StringMap<LineJoin, LINE_JOIN_MAX_ENUM>::Entry lineJoinEntries[];
	static StringMap<LineJoin, LINE_JOIN_MAX_ENUM> lineJoins;

	static StringMap<PointStyle, POINT_MAX_ENUM>::Entry pointStyleEntries[];
	static StringMap<PointStyle, POINT_MAX_ENUM> pointStyles;

	static StringMap<Support, SUPPORT_MAX_ENUM>::Entry supportEntries[];
	static StringMap<Support, SUPPORT_MAX_ENUM> support;

	static StringMap<RendererInfo, RENDERER_INFO_MAX_ENUM>::Entry rendererInfoEntries[];
	static StringMap<RendererInfo, RENDERER_INFO_MAX_ENUM> rendererInfo;

}; // Graphics

} // graphics
} // love

#endif // LOVE_GRAPHICS_GRAPHICS_H
