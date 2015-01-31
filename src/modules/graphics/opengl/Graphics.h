/**
 * Copyright (c) 2006-2015 LOVE Development Team
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

#ifndef LOVE_GRAPHICS_OPENGL_GRAPHICS_H
#define LOVE_GRAPHICS_OPENGL_GRAPHICS_H

// STD
#include <stack>
#include <vector>

// OpenGL
#include "OpenGL.h"

// LOVE
#include "graphics/Graphics.h"
#include "graphics/Color.h"

#include "image/Image.h"
#include "image/ImageData.h"

#include "window/Window.h"

#include "Font.h"
#include "Image.h"
#include "graphics/Quad.h"
#include "SpriteBatch.h"
#include "ParticleSystem.h"
#include "Canvas.h"
#include "Shader.h"
#include "Mesh.h"

namespace love
{
namespace graphics
{
namespace opengl
{

// During display mode changing, certain
// variables about the OpenGL context are
// lost.


class Graphics : public love::graphics::Graphics
{
public:

	Graphics();
	virtual ~Graphics();

	// Implements Module.
	const char *getName() const;

	virtual void setViewportSize(int width, int height);
	virtual bool setMode(int width, int height, bool &sRGB);
	virtual void unSetMode();

	void setDebug(bool enable);

	/**
	 * Resets the current color, background color,
	 * line style, and so forth. (This will be called
	 * when the game reloads.
	 **/
	void reset();

	/**
	 * Clears the screen.
	 **/
	void clear();

	/**
	 * Flips buffers. (Rendered geometry is presented on screen).
	 **/
	void present();

	/**
	 * Gets the width of the current graphics viewport.
	 **/
	int getWidth() const;

	/**
	 * Gets the height of the current graphics viewport.
	 **/
	int getHeight() const;

	/**
	 * True if a graphics viewport is set.
	 **/
	bool isCreated() const;

	/**
	 * Scissor defines a box such that everything outside that box is discarded and not drawn.
	 * Scissoring is automatically enabled.
	 * @param x The x-coordinate of the top-left corner.
	 * @param y The y-coordinate of the top-left corner.
	 * @param width The width of the box.
	 * @param height The height of the box.
	 **/
	void setScissor(int x, int y, int width, int height);

	/**
	 * Clears any scissor that has been created.
	 **/
	void setScissor();

	/**
	 * Gets the current scissor box.
	 * @return Whether the scissor is enabled.
	 */
	bool getScissor(int &x, int &y, int &width, int &height) const;

	/**
	 * Enables the stencil buffer and set stencil function to fill it
	 */
	void defineStencil();

	/**
	 * Set stencil function to mask the following drawing calls using
	 * the current stencil buffer
	 * @param invert Invert the mask, i.e. draw everywhere expect where
	 *               the mask is defined.
	 */
	void useStencil(bool invert = false);

	/**
	 * Disables the stencil buffer
	 */
	void discardStencil();

	/**
	 * Creates an Image object with padding and/or optimization.
	 **/
	Image *newImage(love::image::ImageData *data, Image::Format format = Image::FORMAT_NORMAL);
	Image *newImage(love::image::CompressedData *cdata, Image::Format format = Image::FORMAT_NORMAL);

	Quad *newQuad(Quad::Viewport v, float sw, float sh);

	/**
	 * Creates a Font object.
	 **/
	Font *newFont(love::font::Rasterizer *data, const Texture::Filter &filter = Texture::getDefaultFilter());

	SpriteBatch *newSpriteBatch(Texture *texture, int size, int usage);

	ParticleSystem *newParticleSystem(Texture *texture, int size);

	Canvas *newCanvas(int width, int height, Canvas::Format format = Canvas::FORMAT_NORMAL, int msaa = 0);

	Shader *newShader(const Shader::ShaderSource &source);

	Mesh *newMesh(const std::vector<Vertex> &vertices, Mesh::DrawMode mode = Mesh::DRAW_MODE_FAN);
	Mesh *newMesh(int vertexcount, Mesh::DrawMode mode = Mesh::DRAW_MODE_FAN);

	/**
	 * Sets the foreground color.
	 * @param c The new foreground color.
	 **/
	void setColor(const Color &c);

	/**
	 * Gets current color.
	 **/
	Color getColor() const;

	/**
	 * Sets the background Color.
	 **/
	void setBackgroundColor(const Color &c);

	/**
	 * Gets the current background color.
	 **/
	Color getBackgroundColor() const;

	void setFont(Font *font);
	Font *getFont();

	void setShader(Shader *shader);
	void setShader();

	Shader *getShader() const;

	void setCanvas(Canvas *canvas);
	void setCanvas(const std::vector<Canvas *> &canvases);
	void setCanvas(const std::vector<StrongRef<Canvas>> &canvases);
	void setCanvas();

	std::vector<Canvas *> getCanvas() const;

	/**
	 * Sets the enabled color components when rendering.
	 **/
	void setColorMask(ColorMask mask);

	/**
	 * Gets the current color mask.
	 **/
	ColorMask getColorMask() const;

	/**
	 * Sets the current blend mode.
	 **/
	void setBlendMode(BlendMode mode);

	/**
	 * Gets the current blend mode.
	 **/
	BlendMode getBlendMode() const;

	/**
	 * Sets the default filter for images, canvases, and fonts.
	 **/
	void setDefaultFilter(const Texture::Filter &f);

	/**
	 * Gets the default filter for images, canvases, and fonts.
	 **/
	const Texture::Filter &getDefaultFilter() const;

	/**
	 * Default Image mipmap filter mode and sharpness values.
	 **/
	void setDefaultMipmapFilter(Texture::FilterMode filter, float sharpness);
	void getDefaultMipmapFilter(Texture::FilterMode *filter, float *sharpness) const;

	/**
	 * Sets the line width.
	 * @param width The new width of the line.
	 **/
	void setLineWidth(float width);

	/**
	 * Sets the line style.
	 * @param style LINE_ROUGH or LINE_SMOOTH.
	 **/
	void setLineStyle(LineStyle style);

	/**
	 * Sets the line style.
	 * @param style LINE_ROUGH or LINE_SMOOTH.
	 **/
	void setLineJoin(LineJoin style);

	/**
	 * Gets the line width.
	 **/
	float getLineWidth() const;

	/**
	 * Gets the line style.
	 **/
	LineStyle getLineStyle() const;

	/**
	 * Gets the line style.
	 **/
	LineJoin getLineJoin() const;

	/**
	 * Sets the size of points.
	 **/
	void setPointSize(float size);

	/**
	 * Sets the style of points.
	 * @param style POINT_SMOOTH or POINT_ROUGH.
	 **/
	void setPointStyle(PointStyle style);

	/**
	 * Gets the point size.
	 **/
	float getPointSize() const;

	/**
	 * Gets the point style.
	 **/
	PointStyle getPointStyle() const;

	/**
	 * Sets whether graphics will be drawn as wireframe lines instead of filled
	 * triangles (has no effect for drawn points.)
	 * This should only be used as a debugging tool. The wireframe lines do not
	 * behave the same as regular love.graphics lines.
	 **/
	void setWireframe(bool enable);

	/**
	 * Gets whether wireframe drawing mode is enabled.
	 **/
	bool isWireframe() const;

	/**
	 * Draws text at the specified coordinates, with rotation and
	 * scaling along both axes.
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 * @param angle The amount of rotation.
	 * @param sx The scale factor along the x-axis. (1 = normal).
	 * @param sy The scale factor along the y-axis. (1 = normal).
	 * @param ox The origin offset along the x-axis.
	 * @param oy The origin offset along the y-axis.
	 * @param kx Shear along the x-axis.
	 * @param ky Shear along the y-axis.
	 **/
	void print(const std::string &str, float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

	/**
	 * Draw formatted text on screen at the specified coordinates.
	 *
	 * @param str A string of text.
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 * @param wrap The maximum width of the text area.
	 * @param align Where to align the text.
	 * @param angle The amount of rotation.
	 * @param sx The scale factor along the x-axis. (1 = normal).
	 * @param sy The scale factor along the y-axis. (1 = normal).
	 * @param ox The origin offset along the x-axis.
	 * @param oy The origin offset along the y-axis.
	 * @param kx Shear along the x-axis.
	 * @param ky Shear along the y-axis.
	 **/
	void printf(const std::string &str, float x, float y, float wrap, AlignMode align, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

	/**
	 * Draws a point at (x,y).
	 * @param x Point along x-axis.
	 * @param y Point along y-axis.
	 **/
	void point(float x, float y);

	/**
	 * Draws a series of lines connecting the given vertices.
	 * @param coords Vertex components (x1, y1, ..., xn, yn). If x1,y1 == xn,yn the line will be drawn closed.
	 * @param count Number of items in the array, i.e. count = 2 * n
	 **/
	void polyline(const float *coords, size_t count);

	/**
	 * Draws a rectangle.
	 * @param x Position along x-axis for top-left corner.
	 * @param y Position along y-axis for top-left corner.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 **/
	void rectangle(DrawMode mode, float x, float y, float w, float h);

	/**
	 * Draws a circle using the specified arguments.
	 * @param mode The mode of drawing (line/filled).
	 * @param x X-coordinate.
	 * @param y Y-coordinate.
	 * @param radius Radius of the circle.
	 * @param points Number of points to use to draw the circle.
	 **/
	void circle(DrawMode mode, float x, float y, float radius, int points = 10);

	/**
	 * Draws an arc using the specified arguments.
	 * @param mode The mode of drawing (line/filled).
	 * @param x X-coordinate.
	 * @param y Y-coordinate.
	 * @param radius Radius of the arc.
	 * @param angle1 The angle at which the arc begins.
	 * @param angle2 The angle at which the arc terminates.
	 * @param points Number of points to use to draw the arc.
	 **/
	void arc(DrawMode mode, float x, float y, float radius, float angle1, float angle2, int points = 10);

	/**
	 * Draws a polygon with an arbitrary number of vertices.
	 * @param mode The type of drawing (line/filled).
	 * @param coords Vertex components (x1, y1, x2, y2, etc.)
	 * @param count Coord array size
	 **/
	void polygon(DrawMode mode, const float *coords, size_t count);

	/**
	 * Creates a screenshot of the view and saves it to the default folder.
	 * @param image The love.image module.
	 * @param copyAlpha If the alpha channel should be copied or set to full opacity (255).
	 **/
	love::image::ImageData *newScreenshot(love::image::Image *image, bool copyAlpha = true);

	/**
	 * Returns system-dependent renderer information.
	 * Returned strings can vary greatly between systems! Do not rely on it for
	 * anything!
	 **/
	RendererInfo getRendererInfo() const;

	/**
	 * Returns performance-related statistics.
	 **/
	Stats getStats() const;

	/**
	 * Gets the system-dependent numeric limit for the specified parameter.
	 **/
	double getSystemLimit(SystemLimit limittype) const;

	/**
	 * Gets whether a graphics feature is supported on this system.
	 **/
	bool isSupported(Support feature) const;

	void push(StackType type = STACK_TRANSFORM);
	void pop();

	void rotate(float r);
	void scale(float x, float y = 1.0f);
	void translate(float x, float y);
	void shear(float kx, float ky);
	void origin();

private:

	struct DisplayState
	{
		Color color;
		Color backgroundColor;

		BlendMode blendMode;

		float lineWidth;
		LineStyle lineStyle;
		LineJoin lineJoin;

		float pointSize;
		PointStyle pointStyle;

		bool scissor;
		OpenGL::Viewport scissorBox;

		StrongRef<Font> font;
		StrongRef<Shader> shader;

		std::vector<StrongRef<Canvas>> canvases;

		ColorMask colorMask;

		bool wireframe;

		Texture::Filter defaultFilter;

		Texture::FilterMode defaultMipmapFilter;
		float defaultMipmapSharpness;

		DisplayState();
		DisplayState(const DisplayState &other);
		~DisplayState();

		DisplayState &operator = (const DisplayState &other);
	};

	void restoreState(const DisplayState &s);
	void restoreStateChecked(const DisplayState &s);

	void checkSetDefaultFont();

	love::window::Window *currentWindow;

	StrongRef<Font> defaultFont;

	std::vector<double> pixel_size_stack; // stores current size of a pixel (needed for line drawing)

	int width;
	int height;
	bool created;

	bool activeStencil;

	std::vector<DisplayState> states;
	std::vector<StackType> stackTypes; // Keeps track of the pushed stack types.

	static const size_t MAX_USER_STACK_DEPTH = 64;

}; // Graphics

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_GRAPHICS_H
