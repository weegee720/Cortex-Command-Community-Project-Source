#ifndef _RTETOOLS_
#define _RTETOOLS_

// Header file for global utility methods.

#include "RTEError.h"
#include "Constants.h"

namespace RTE {

	class Vector;

	extern std::mt19937 g_RNG; //!< The random number generator used for all random functions.

#pragma region Physics Constants Getters
	/// <summary>
	/// Gets the ratio between the physics engine's meters and on-screen pixels.
	/// </summary>
	/// <returns>A float describing the current MPP ratio.</returns>
	float GetMPP();

	/// <summary>
	/// Gets the ratio between on-screen pixels and the physics engine's meters.
	/// </summary>
	/// <returns>A float describing the current PPM ratio.</returns>
	float GetPPM();

	/// <summary>
	/// Gets the ratio between the physics engine's Liters and on-screen pixels.
	/// </summary>
	/// <returns>A float describing the current LPP ratio.</returns>
	float GetLPP();

	/// <summary>
	/// Gets the ratio between the on-screen pixels and the physics engine's Liters.
	/// </summary>
	/// <returns>A float describing the current PPL ratio.</returns>
	float GetPPL();
#pragma endregion

#pragma region Random Numbers
	/// <summary>
	/// Seed the mt19937 random number generator. mt19937 is the standard mersenne_twister_engine.
	/// </summary>
	void SeedRNG();

	/// <summary>
	/// Seed the mt19937 random number generator. mt19937 is the standard mersenne_twister_engine.
	/// </summary>
	/// <param name="seed">Seed for the random number generator.</param>
	void SeedRNG(unsigned int seed);

	/// <summary>
	/// Function template which returns a uniformly distributed random number in the range [-1, 1].
	/// </summary>
	/// <returns>Uniformly distributed random number in the range [-1, 1].</returns>
	template <typename floatType = float>
	typename std::enable_if<std::is_floating_point<floatType>::value, floatType>::type RandomNormalNum() {
		return std::uniform_real_distribution<floatType>(floatType(-1.0), std::nextafter(floatType(1.0), std::numeric_limits<floatType>::max()))(g_RNG);
	}

	/// <summary>
	/// Function template specialization for int types which returns a uniformly distributed random number in the range [-1, 1].
	/// </summary>
	/// <returns>Uniformly distributed random number in the range [-1, 1].</returns>
	template <typename intType>
	typename std::enable_if<std::is_integral<intType>::value, intType>::type RandomNormalNum() {
		return std::uniform_int_distribution<intType>(intType(-1), intType(1))(g_RNG);
	}

	/// <summary>
	/// Function template which returns a uniformly distributed random number in the range [0, 1].
	/// </summary>
	/// <returns>Uniformly distributed random number in the range [0, 1].</returns>
	template <typename floatType = float>
	typename std::enable_if<std::is_floating_point<floatType>::value, floatType>::type RandomNum() {
		return std::uniform_real_distribution<floatType>(floatType(0.0), std::nextafter(floatType(1.0), std::numeric_limits<floatType>::max()))(g_RNG);
	}

	/// <summary>
	/// Function template specialization for int types which returns a uniformly distributed random number in the range [0, 1].
	/// </summary>
	/// <returns>Uniformly distributed random number in the range [0, 1].</returns>
	template <typename intType>
	typename std::enable_if<std::is_integral<intType>::value, intType>::type RandomNum() {
		return std::uniform_int_distribution<intType>(intType(0), intType(1))(g_RNG);
	}

	/// <summary>
	/// Function template which returns a uniformly distributed random number in the range [min, max].
	/// </summary>
	/// <param name="min">Lower boundary of the range to pick a number from.</param>
	/// <param name="max">Upper boundary of the range to pick a number from.</param>
	/// <returns>Uniformly distributed random number in the range [min, max].</returns>
	template <typename floatType = float>
	typename std::enable_if<std::is_floating_point<floatType>::value, floatType>::type RandomNum(floatType min, floatType max) {
		if (max < min) { std::swap(min, max); }
		return (std::uniform_real_distribution<floatType>(floatType(0.0), std::nextafter(max - min, std::numeric_limits<floatType>::max()))(g_RNG) + min);
	}

	/// <summary>
	/// Function template specialization for int types which returns a uniformly distributed random number in the range [min, max].
	/// </summary>
	/// <param name="min">Lower boundary of the range to pick a number from.</param>
	/// <param name="max">Upper boundary of the range to pick a number from.</param>
	/// <returns>Uniformly distributed random number in the range [min, max].</returns>
	template <typename intType>
	typename std::enable_if<std::is_integral<intType>::value, intType>::type RandomNum(intType min, intType max) {
		if (max < min) { std::swap(min, max); }
		return (std::uniform_int_distribution<intType>(intType(0), max - min)(g_RNG) + min);
	}
#pragma endregion

#pragma region Interpolation
	/// <summary>
	/// Simple Linear Interpolation, with an added bonus: scaleStart and scaleEnd let you define your scale, where 0 and 1 would be standard scale.
	/// This scale is used to normalize your progressScalar value and lerp accordingly.
	/// </summary>
	/// <param name="scaleStart">The start of the scale to lerp along.</param>
	/// <param name="scaleEnd">The end of the scale to lerp along.</param>
	/// <param name="startValue">The start value of your lerp.</param>
	/// <param name="endValue">The end value of your lerp.</param>
	/// <param name="progressScalar">How far your lerp has progressed. Automatically normalized through use of scaleStart and scaleEnd.</param>
	/// <returns>Interpolated value.</returns>
	float LERP(float scaleStart, float scaleEnd, float startValue, float endValue, float progressScalar);

	/// <summary>
	/// Nonlinear ease-in interpolation. Starts slow.
	/// </summary>
	/// <param name="start">Start value.</param>
	/// <param name="end">End value.</param>
	/// <param name="progressScalar">Normalized positive progress scalar (0 - 1.0).</param>
	/// <returns>Interpolated value.</returns>
	float EaseIn(float start, float end, float progressScalar);

	/// <summary>
	/// Nonlinear ease-out interpolation. Slows down toward the end.
	/// </summary>
	/// <param name="start">Start value.</param>
	/// <param name="end">End value.</param>
	/// <param name="progressScalar">Normalized positive progress scalar (0 - 1.0).</param>
	/// <returns>Interpolated value.</returns>
	float EaseOut(float start, float end, float progressScalar);

	/// <summary>
	/// Nonlinear ease-in-out interpolation. Slows down in the start and end.
	/// </summary>
	/// <param name="start">Start value.</param>
	/// <param name="end">End value.</param>
	/// <param name="progressScalar">Normalized positive progress scalar (0 - 1.0).</param>
	/// <returns>Interpolated value.</returns>
	float EaseInOut(float start, float end, float progressScalar);
#pragma endregion

#pragma region Clamping
	/// <summary>
	/// Clamps a value between two limit values.
	/// </summary>
	/// <param name="value">Value to clamp.</param>
	/// <param name="upperLimit">Upper limit of value.</param>
	/// <param name="lowerLimit">Lower limit of value.</param>
	/// <returns>True if either limit is currently reached, False if not.</returns>
	bool Clamp(float &value, float upperLimit, float lowerLimit);

	/// <summary>
	/// Clamps a value between two limit values.
	/// </summary>
	/// <param name="value">Value to clamp.</param>
	/// <param name="upperLimit">Upper limit of value.</param>
	/// <param name="lowerLimit">Lower limit of value.</param>
	/// <returns>Upper/Lower limit value if limit is currently reached, value between limits if not.</returns>
	float Limit(float value, float upperLimit, float lowerLimit);
#pragma endregion

#pragma region Detection
	/// <summary>
	/// Tells whether a point is within a specified box.
	/// </summary>
	/// <param name="point">Vector position of the point we're checking.</param>
	/// <param name="boxPos">Vector position of the box.</param>
	/// <param name="width">Width of the box.</param>
	/// <param name="height">Height of the box.</param>
	/// <returns>True if point is inside box bounds.</returns>
	bool WithinBox(Vector &point, Vector &boxPos, float width, float height);

	/// <summary>
	/// Tells whether a point is within a specified box.
	/// </summary>
	/// <param name="point">Vector position of the point we're checking.</param>
	/// <param name="left">Position of box left plane (X start).</param>
	/// <param name="top">Position of box top plane (Y start).</param>
	/// <param name="right">Position of box right plane (X end).</param>
	/// <param name="bottom">Position of box bottom plane (Y end).</param>
	/// <returns>True if point is inside box bounds.</returns>
	bool WithinBox(Vector &point, float left, float top, float right, float bottom);
#pragma endregion

#pragma region Conversion
	/// <summary>
	/// Returns a corrected angle value that can be used with Allegro fixed point math routines where 256 equals 360 degrees.
	/// </summary>
	/// <param name="angleDegrees">The angle value to correct. In degrees.</param>
	/// <returns>A float with the represented angle as full rotations being 256.</returns>
	float GetAllegroAngle(float angleDegrees);
#pragma endregion

#pragma region Misc
	/// <summary>
	/// Fires up the default browser for the current OS on a specific URL.
	/// </summary>
	/// <param name="goToURL">A string with the URL to send the browser to.</param>
	void OpenBrowserToURL(std::string goToURL);

	/// <summary>
	/// Searches through an ASCII file on disk for a specific string and tells whether it was found or not.
	/// </summary>
	/// <param name="">The path to the ASCII file to search.</param>
	/// <param name="">The exact string to look for. Case sensitive!</param>
	/// <returns>Whether the file was found AND that string was found in that file.</returns>
	bool ASCIIFileContainsString(std::string filePath, std::string findString);
#pragma endregion
}
#endif