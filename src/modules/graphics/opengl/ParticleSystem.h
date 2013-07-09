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

#ifndef LOVE_GRAPHICS_OPENGL_PARTICLE_SYSTEM_H
#define LOVE_GRAPHICS_OPENGL_PARTICLE_SYSTEM_H

// LOVE
#include "common/int.h"
#include "common/math.h"
#include "common/Vector.h"
#include "graphics/Drawable.h"
#include "graphics/Color.h"
#include "Image.h"

// STL
#include <vector>

namespace love
{
namespace graphics
{
namespace opengl
{

/**
 * A class for creating, moving and drawing particles.
 * A big thanks to bobthebloke.org
 **/
class ParticleSystem : public Drawable
{
public:
	/**
	 * Type of distribution new particles are drawn from: None, uniform, normal.
	 */
	enum AreaSpreadDistribution
	{
		DISTRIBUTION_NONE,
		DISTRIBUTION_UNIFORM,
		DISTRIBUTION_NORMAL,
		DISTRIBUTION_MAX_ENUM
	};

	/**
	 * Insertion modes of new particles in the list: top, bottom, random.
	 */
	enum InsertMode
	{
		INSERT_MODE_TOP,
		INSERT_MODE_BOTTOM,
		INSERT_MODE_RANDOM,
		INSERT_MODE_MAX_ENUM,
	};

	/**
	 * Maximum numbers of particles in a ParticleSystem.
	 * This limit comes from the fact that a quad requires four vertices and the
	 * OpenGL API where GLsizei is a signed int.
	 **/
	static const uint32 MAX_PARTICLES = LOVE_INT32_MAX / 4;

	/**
	 * Creates a particle system with the specified buffersize and image.
	 **/
	ParticleSystem(Image *image, uint32 buffer);

	/**
	 * Deletes any allocated memory.
	 **/
	virtual ~ParticleSystem();

	/**
	 * Sets the image used in the particle system.
	 * @param image The new image.
	 **/
	void setImage(Image *image);

	/**
	 * Returns the image used when drawing the particle system.
	 **/
	Image *getImage() const;

	/**
	 * Clears the current buffer and allocates the appropriate amount of space for the buffer.
	 * @param size The new buffer size.
	 **/
	void setBufferSize(uint32 size);

	/**
	 * Returns the total amount of particles this ParticleSystem can have active
	 * at any given point in time.
	 **/
	uint32 getBufferSize() const;

	/**
	 * Sets the insert mode for new particles.
	 * @param mode The new insert mode.
	 */
	void setInsertMode(InsertMode mode);

	/**
	 * Returns the current insert mode.
	 * @param mode The current insert mode.
	 */
	InsertMode getInsertMode() const;

	/**
	 * Sets the emission rate.
	 * @param rate The amount of particles per second.
	 **/
	void setEmissionRate(int rate);

	/**
	 * Returns the number of particles created per second.
	 **/
	int getEmissionRate() const;

	/**
	 * Sets the lifetime of the particle emitter (-1 means eternal)
	 * @param life The lifetime (in seconds).
	 **/
	void setEmitterLifetime(float life);

	/**
	 * Returns the lifetime of the particle emitter.
	 **/
	float getEmitterLifetime() const;

	/**
	 * Sets the life range of the particles.
	 * @param lifeMin The minimum life.
	 * @param lifeMax The maximum life (if 0, then becomes the same as minimum life).
	 **/
	void setParticleLifetime(float min, float max = 0);

	/**
	 * Gets the lifetime of a particle.
	 * @param[out] min
	 * @param[out] max
	 **/
	void getParticleLifetime(float *min, float *max) const;

	/**
	 * Sets the position of the center of the emitter and the direction (if set to relative).
	 * Used to move the emitter without changing the position of already existing particles.
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 **/
	void setPosition(float x, float y);

	/**
	 * Returns the position of the emitter.
	 **/
	const love::Vector &getPosition() const;

	/**
	 * Returns the x-coordinate of the emitter's position.
	 **/
	float getX() const;

	/**
	 * Returns the y-coordinate of the emitter's position.
	 **/
	float getY() const;

	/**
	 * Sets the emission area spread parameters and distribution type. The interpretation of
	 * the parameters depends on the distribution type:
	 *
	 * * None:    Parameters are ignored. No area spread.
	 * * Uniform: Parameters denote maximal (symmetric) displacement from emitter position.
	 * * Normal:  Parameters denote the standard deviation in x and y direction. x and y are assumed to be uncorrelated.
	 * @param x First parameter. Interpretation depends on distribution type.
	 * @param y Second parameter. Interpretation depends on distribution type.
	 * @param distribution Distribution type
	 **/
	void setAreaSpread(AreaSpreadDistribution distribution, float x, float y);

	/**
	 * Returns area spread distribution type.
	 **/
	AreaSpreadDistribution getAreaSpreadDistribution() const;

	/**
	 * Returns area spread parameters.
	 **/
	const love::Vector &getAreaSpreadParameters() const;

	/**
	 * Sets the direction of the particle emitter.
	 * @param direction The direction (in degrees).
	 **/
	void setDirection(float direction);

	/**
	 * Returns the direction of the particle emitter (in radians).
	 **/
	float getDirection() const;

	/**
	 * Sets the spread of the particle emitter.
	 * @param spread The spread (in radians).
	 **/
	void setSpread(float spread);

	/**
	 * Returns the directional spread of the emitter (in radians).
	 **/
	float getSpread() const;

	/**
	 * Sets whether the direction should be relative to the particle emitter's movement. Used in conjunction with setPosition.
	 * @param relative Whether to have relative direction.
	 **/
	void setRelativeDirection(bool relative);

	/**
	 * Returns whether the direction is relative to the particle emitter's
	 * movement.
	 **/
	bool isRelativeDirection() const;

	/**
	 * Sets the speed of the particles.
	 * @param speed The speed.
	 **/
	void setSpeed(float speed);

	/**
	 * Sets the speed of the particles.
	 * @param min The minimum speed.
	 * @param max The maximum speed.
	 **/
	void setSpeed(float min, float max);

	/**
	 * Gets the speed of the particles.
	 * @param[out] min
	 * @param[out] max
	 **/
	void getSpeed(float *min, float *max) const;

	/**
	 * Sets the linear acceleration (the acceleration along the x and y axes).
	 * @param x The acceleration along the x-axis.
	 * @param y The acceleration along the y-axis.
	 **/
	void setLinearAcceleration(float x, float y);

	/**
	 * Sets the linear acceleration (the acceleration along the x and y axes).
	 * @param xmin The minimum amount of acceleration along the x-axis.
	 * @param ymin The minimum amount of acceleration along the y-axis.
	 * @param xmax The maximum amount of acceleration along the x-axis.
	 * @param ymax The maximum amount of acceleration along the y-axis.
	 **/
	void setLinearAcceleration(float xmin, float ymin, float xmax, float ymax);

	/**
	 * Gets the linear acceleration of the particles.
	 * @param[out] min The minimum acceleration.
	 * @param[out] max The maximum acceleration.
	 **/
	void getLinearAcceleration(love::Vector *min, love::Vector *max) const;

	/**
	 * Sets the radial acceleration (the acceleration towards the particle emitter).
	 * @param acceleration The amount of acceleration.
	 **/
	void setRadialAcceleration(float acceleration);

	/**
	 * Sets the radial acceleration (the acceleration towards the particle emitter).
	 * @param min The minimum acceleration.
	 * @param max The maximum acceleration.
	 **/
	void setRadialAcceleration(float min, float max);

	/**
	 * Gets the radial acceleration.
	 * @param[out] min
	 * @param[out] max
	 **/
	void getRadialAcceleration(float *min, float *max) const;

	/**
	 * Sets the tangential acceleration (the acceleration perpendicular to the particle's direction).
	 * @param acceleration The amount of acceleration.
	 **/
	void setTangentialAcceleration(float acceleration);

	/**
	 * Sets the tangential acceleration (the acceleration perpendicular to the particle's direction).
	 * @param min The minimum acceleration.
	 * @param max The maximum acceleration.
	 **/
	void setTangentialAcceleration(float min, float max);

	/**
	 * Gets the tangential acceleration.
	 * @param[out] min
	 * @param[out] max
	 **/
	void getTangentialAcceleration(float *min, float *max) const;

	/**
	 * Sets the size of the sprite (1.0 being the default size).
	 * @param size The size of the sprite.
	 **/
	void setSize(float size);

	/**
	 * Sets the sizes of the sprite upon creation and upon death (1.0 being the default size).
	 * @param newSizes Array of sizes
	 **/
	void setSizes(const std::vector<float> &newSizes);

	/**
	 * Returns the sizes of the particle sprites.
	 **/
	const std::vector<float> &getSizes() const;

	/**
	 * Sets the amount of variation to the sprite's beginning size (0 being no variation and 1.0 a random size between start and end).
	 * @param variation The amount of variation.
	 **/
	void setSizeVariation(float variation);

	/**
	 * Returns the amount of initial size variation between particles.
	 **/
	float getSizeVariation() const;

	/**
	 * Sets the amount of rotation a sprite starts out with.
	 * @param rotation The amount of rotation.
	 **/
	void setRotation(float rotation);

	/**
	 * Sets the amount of rotation a sprite starts out with (a random value between min and max).
	 * @param min The minimum amount of rotation.
	 * @param max The maximum amount of rotation.
	 **/
	void setRotation(float min, float max);

	/**
	 * Gets the initial amount of rotation of a particle.
	 * @param[out] min
	 * @param[out] max
	 **/
	void getRotation(float *min, float *max) const;

	/**
	 * Sets the spin of the sprite.
	 * @param spin The spin of the sprite (in degrees).
	 **/
	void setSpin(float spin);

	/**
	 * Sets the spin of the sprite upon particle creation and death.
	 * @param start The spin of the sprite upon creation (in radians / second).
	 * @param end The spin of the sprite upon death (in radians / second).
	 **/
	void setSpin(float start, float end);

	/**
	 * Gets the amount of spin of a particle during its lifetime.
	 * @param[out] start
	 * @param[out] end
	 **/
	void getSpin(float *start, float *end) const;

	/**
	 * Sets the variation of the start spin (0 being no variation and 1 being a random spin between start and end).
	 * @param variation The variation.
	 **/
	void setSpinVariation(float variation);

	/**
	 * Returns the amount of variation of the start spin of a particle.
	 **/
	float getSpinVariation() const;

	/**
	 * Sets the particles' offsets for rotation.
	 * @param x The x offset.
	 * @param y The y offset.
	 **/
	void setOffset(float x, float y);

	/**
	 * Returns of the particle offset.
	 **/
	love::Vector getOffset() const;

	/**
	 * Sets the color of the particles.
	 * @param color The color.
	 **/
	void setColor(const Color &color);

	/**
	 * Sets the color of the particles.
	 * @param newColors Array of colors
	 **/
	void setColor(const std::vector<Color> &newColors);

	/**
	 * Returns the color of the particles.
	 **/
	std::vector<Color> getColor() const;

	/**
	 * Returns the amount of particles that are currently active in the system.
	 **/
	uint32 getCount() const;

	/**
	 * Starts/resumes the particle emitter.
	 **/
	void start();

	/**
	 * Stops the particle emitter and resets.
	 **/
	void stop();

	/**
	 * Pauses the particle emitter.
	 **/
	void pause();

	/**
	 * Resets the particle emitter.
	 **/
	void reset();

	/**
	 * Instantly emits a number of particles.
	 * @param num The number of particles to emit.
	 **/
	void emit(uint32 num);

	/**
	 * Returns whether the particle emitter is active.
	 **/
	bool isActive() const;

	/**
	 * Returns whether the particle emitter is paused.
	 **/
	bool isPaused() const;

	bool isStopped() const;

	/**
	 * Returns whether the particle system is empty of particles or not.
	 **/
	bool isEmpty() const;

	/**
	 * Returns whether the amount of particles has reached the buffer limit or not.
	 **/
	bool isFull() const;

	/**
	 * Draws the particle emitter at the specified position.
	 * @param x The x-coordinate.
	 * @param y The y-coordinate.
	 **/
	virtual void draw(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky) const;

	/**
	 * Updates the particle system.
	 * @param dt Time since last update.
	 **/
	void update(float dt);

	static bool getConstant(const char *in, AreaSpreadDistribution &out);
	static bool getConstant(AreaSpreadDistribution in, const char *&out);

	static bool getConstant(const char *in, InsertMode &out);
	static bool getConstant(InsertMode in, const char *&out);

protected:
	// Represents a single particle.
	struct particle
	{
		particle *prev;
		particle *next;

		float lifetime;
		float life;

		float position[2];
		float direction;

		// Particles gravitate towards this point.
		love::Vector origin;

		love::Vector speed;
		love::Vector linearAcceleration;
		float radialAcceleration;
		float tangentialAcceleration;

		float size;
		float sizeOffset;
		float sizeIntervalSize;

		float rotation;
		float spinStart;
		float spinEnd;

		Colorf color;
	};

	// The max amount of particles.
	int bufferSize;

	// Pointer to the beginning of the allocated memory.
	particle *pMem;

	// Pointer to a free particle.
	particle *pFree;

	// Pointer to the start of the linked list.
	particle *pHead;

	// Pointer to the end of the linked list.
	particle *pTail;

	// array of transformed vertex data for all particles, for drawing
	vertex * particleVerts;

	// The image to be drawn.
	Image *image;

	// Whether the particle emitter is active.
	bool active;

	// Insert mode of new particles.
	InsertMode insertMode;

	// The maximum number of particles.
	uint32 maxParticles;

	// The number of active particles.
	uint32 activeParticles;

	// The emission rate (particles/sec).
	int emissionRate;

	// Used to determine when a particle should be emitted.
	float emitCounter;

	// The relative position of the particle emitter.
	love::Vector position;

	// Emission area spread.
	AreaSpreadDistribution areaSpreadDistribution;
	love::Vector areaSpread;

	// The lifetime of the particle emitter (-1 means infinite) and the life it has left.
	float lifetime;
	float life;

	// The particle life.
	float particleLifeMin;
	float particleLifeMax;

	// The direction (and spread) the particles will be emitted in. Measured in radians.
	float direction;
	float spread;

	// Whether the direction should be relative to the emitter's movement.
	bool relative;

	// The speed.
	float speedMin;
	float speedMax;

	// Acceleration along the x and y axes.
	love::Vector linearAccelerationMin;
	love::Vector linearAccelerationMax;

	// Acceleration towards the emitter's center
	float radialAccelerationMin;
	float radialAccelerationMax;

	// Acceleration perpendicular to the particle's direction.
	float tangentialAccelerationMin;
	float tangentialAccelerationMax;

	// Size.
	std::vector<float> sizes;
	float sizeVariation;

	// Rotation
	float rotationMin;
	float rotationMax;

	// Spin.
	float spinStart;
	float spinEnd;
	float spinVariation;

	// Offsets
	float offsetX;
	float offsetY;

	// Color.
	std::vector<Colorf> colors;

	void createBuffers(size_t size);
	void deleteBuffers();

	void addParticle();
	particle *removeParticle(particle *p);

	// Called by addParticle.
	void initParticle(particle *p);
	void insertTop(particle *p);
	void insertBottom(particle *p);
	void insertRandom(particle *p);

	static StringMap<AreaSpreadDistribution, DISTRIBUTION_MAX_ENUM>::Entry distributionsEntries[];
	static StringMap<AreaSpreadDistribution, DISTRIBUTION_MAX_ENUM> distributions;

	static StringMap<InsertMode, INSERT_MODE_MAX_ENUM>::Entry insertModesEntries[];
	static StringMap<InsertMode, INSERT_MODE_MAX_ENUM> insertModes;
};

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_PARTICLE_SYSTEM_H
