/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* Based on the Adventure Game Studio source code, copyright 1999-2011 Chris Jones,
 * which is licensed under the Artistic License 2.0.
 * You may also modify/distribute the code in this file under that license.
 */

#include "engines/ags/scripting/scripting.h"
#include "common/random.h"

namespace AGS {

// import int GetTime(int whichValue)
// Undocumented.
RuntimeValue Script_GetTime(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int whichValue = params[0]._signedValue;
	UNUSED(whichValue);

	// FIXME
	error("GetTime unimplemented");

	return RuntimeValue();
}

// import int GetRawTime()
// Undocumented.
RuntimeValue Script_GetRawTime(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("GetRawTime unimplemented");

	return RuntimeValue();
}

enum {
	kRoundDown = 0,
	kRoundNearest = 1,
	kRoundUp = 2
};

// import int FloatToInt(float value, RoundDirection=eRoundDown)
// Converts a floating point value to an integer.
RuntimeValue Script_FloatToInt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float value = params[0]._floatValue;
	uint32 roundDirection = params[1]._value;

	if (value >= 0.0) {
		switch (roundDirection) {
		case kRoundDown:
			return (int)value;
		case kRoundNearest:
			return (int)(value + 0.5);
		case kRoundUp:
			return (int)(value + 0.999999);
		}
	} else {
		switch (roundDirection) {
		case kRoundUp:
			return (int)value;
		case kRoundNearest:
			return (int)(value - 0.5);
		case kRoundDown:
			return (int)(value - 0.999999);
		}
	}

	error("FloatToInt: invalid round direction %d", roundDirection);
}

// import float IntToFloat(int value)
// Converts an integer to a floating point number.
RuntimeValue Script_IntToFloat(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int value = params[0]._signedValue;

	return (float)value;
}

// import int Random(int max)
// Returns a random number between 0 and MAX, inclusive.
RuntimeValue Script_Random(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint max = params[0]._value;

	return vm->getRandomSource()->getRandomNumber(max);
}

// Maths: import static float ArcCos(float value)
// Calculates the Arc Cosine of the specified value.
RuntimeValue Script_Maths_ArcCos(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float value = params[0]._floatValue;
	float res = acos(value);
	return RuntimeValue(res);
}

// Maths: import static float ArcSin(float value)
// Calculates the Arc Sine of the specified value.
RuntimeValue Script_Maths_ArcSin(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float value = params[0]._floatValue;
	float res = asin(value);
	return RuntimeValue(res);
}

// Maths: import static float ArcTan(float value)
// Calculates the Arc Tan of the specified value.
RuntimeValue Script_Maths_ArcTan(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float value = params[0]._floatValue;
	value = atan(value);
	return RuntimeValue(value);
}

// Maths: import static float ArcTan2(float y, float x)
// Calculates the Arc Tan of y/x.
RuntimeValue Script_Maths_ArcTan2(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float y = params[0]._floatValue;
	float x = params[1]._floatValue;
	float value = atan2(y, x);
	return RuntimeValue(value);
}

// Maths: import static float Cos(float radians)
// Calculates the cosine of the specified angle.
RuntimeValue Script_Maths_Cos(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;

	return (float)cos(radians);
}

// Maths: import static float Cosh(float radians)
// Calculates the hyperbolic cosine of the specified angle.
RuntimeValue Script_Maths_Cosh(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	float res = cosh(radians);
	return RuntimeValue(res);
}

// Maths: import static float DegreesToRadians(float degrees)
// Converts the angle from degrees to radians.
RuntimeValue Script_Maths_DegreesToRadians(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float degrees = params[0]._floatValue;
	degrees *= (M_PI / 180.0);
	return RuntimeValue(degrees);
}

// Maths: import static float Exp(float x)
// Calculates the value of e to the power x.
RuntimeValue Script_Maths_Exp(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float x = params[0]._floatValue;
	float res = exp(x);
	return RuntimeValue(res);
}

// Maths: import static float Log(float x)
// Calculates the natural logarithm (base e) of x.
RuntimeValue Script_Maths_Log(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float x = params[0]._floatValue;
	x = log(x);
	return RuntimeValue(x);
}

// Maths: import static float Log10(float x)
// Calculates the base-10 logarithm of x.
RuntimeValue Script_Maths_Log10(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float x = params[0]._floatValue;
	float res = log10(x);
	return RuntimeValue(res);
}

// Maths: import static float RadiansToDegrees(float radians)
// Converts the angle from radians to degrees.
RuntimeValue Script_Maths_RadiansToDegrees(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	float degrees = radians * (180.0 / M_PI);
	return RuntimeValue(degrees);
}

// Maths: import static float RaiseToPower(float base, float exponent)
// Calculates the base raised to the power of the exponent.
RuntimeValue Script_Maths_RaiseToPower(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float base = params[0]._floatValue;
	float exponent = params[1]._floatValue;

	return (float)::pow(base, exponent);
}

// Maths: import static float Sin(float radians)
// Calculates the sine of the angle.
RuntimeValue Script_Maths_Sin(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	return (float)sin(radians);
}

// Maths: import static float Sinh(float radians)
// Calculates the hyperbolic sine of the specified angle.
RuntimeValue Script_Maths_Sinh(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	float res = sinh(radians);
	return RuntimeValue(res);
}

// Maths: import static float Sqrt(float value)
// Calculates the square root of the value.
RuntimeValue Script_Maths_Sqrt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float value = params[0]._floatValue;
	value = sqrt(value);
	return RuntimeValue(value);
}

// Maths: import static float Tan(float radians)
// Calculates the tangent of the angle.
RuntimeValue Script_Maths_Tan(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	float res = tan(radians);
	return RuntimeValue(res);
}

// Maths: import static float Tanh(float radians)
// Calculates the hyperbolic tangent of the specified angle.
RuntimeValue Script_Maths_Tanh(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	float radians = params[0]._floatValue;
	float res = tanh(radians);
	return RuntimeValue(res);
}

// Maths: readonly import static attribute float Pi
// Gets the value of PI
RuntimeValue Script_Maths_get_Pi(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	return (float)M_PI;
}

// DateTime: readonly import static attribute DateTime* Now
// Gets the current date and time on the player's system.
RuntimeValue Script_DateTime_get_Now(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Now unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int Year
// Gets the Year component of the date.
RuntimeValue Script_DateTime_get_Year(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Year unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int Month
// Gets the Month (1-12) component of the date.
RuntimeValue Script_DateTime_get_Month(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Month unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int DayOfMonth
// Gets the DayOfMonth (1-31) component of the date.
RuntimeValue Script_DateTime_get_DayOfMonth(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_DayOfMonth unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int Hour
// Gets the Hour (0-23) component of the time.
RuntimeValue Script_DateTime_get_Hour(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Hour unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int Minute
// Gets the Minute (0-59) component of the time.
RuntimeValue Script_DateTime_get_Minute(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Minute unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int Second
// Gets the Second (0-59) component of the time.
RuntimeValue Script_DateTime_get_Second(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_Second unimplemented");

	return RuntimeValue();
}

// DateTime: readonly import attribute int RawTime
// Gets the raw time value, useful for calculating elapsed time periods.
RuntimeValue Script_DateTime_get_RawTime(AGSEngine *vm, DateTime *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("DateTime::get_RawTime unimplemented");

	return RuntimeValue();
}

static const ScriptSystemFunctionInfo ourFunctionList[] = {
	{ "GetTime", (ScriptAPIFunction *)&Script_GetTime, "i", sotNone },
	{ "GetRawTime", (ScriptAPIFunction *)&Script_GetRawTime, "", sotNone },
	{ "FloatToInt", (ScriptAPIFunction *)&Script_FloatToInt, "fi", sotNone },
	{ "IntToFloat", (ScriptAPIFunction *)&Script_IntToFloat, "i", sotNone },
	{ "Random", (ScriptAPIFunction *)&Script_Random, "i", sotNone },
	{ "Maths::ArcCos^1", (ScriptAPIFunction *)&Script_Maths_ArcCos, "f", sotNone },
	{ "Maths::ArcSin^1", (ScriptAPIFunction *)&Script_Maths_ArcSin, "f", sotNone },
	{ "Maths::ArcTan^1", (ScriptAPIFunction *)&Script_Maths_ArcTan, "f", sotNone },
	{ "Maths::ArcTan2^2", (ScriptAPIFunction *)&Script_Maths_ArcTan2, "ff", sotNone },
	{ "Maths::Cos^1", (ScriptAPIFunction *)&Script_Maths_Cos, "f", sotNone },
	{ "Maths::Cosh^1", (ScriptAPIFunction *)&Script_Maths_Cosh, "f", sotNone },
	{ "Maths::DegreesToRadians^1", (ScriptAPIFunction *)&Script_Maths_DegreesToRadians, "f", sotNone },
	{ "Maths::Exp^1", (ScriptAPIFunction *)&Script_Maths_Exp, "f", sotNone },
	{ "Maths::Log^1", (ScriptAPIFunction *)&Script_Maths_Log, "f", sotNone },
	{ "Maths::Log10^1", (ScriptAPIFunction *)&Script_Maths_Log10, "f", sotNone },
	{ "Maths::RadiansToDegrees^1", (ScriptAPIFunction *)&Script_Maths_RadiansToDegrees, "f", sotNone },
	{ "Maths::RaiseToPower^2", (ScriptAPIFunction *)&Script_Maths_RaiseToPower, "ff", sotNone },
	{ "Maths::Sin^1", (ScriptAPIFunction *)&Script_Maths_Sin, "f", sotNone },
	{ "Maths::Sinh^1", (ScriptAPIFunction *)&Script_Maths_Sinh, "f", sotNone },
	{ "Maths::Sqrt^1", (ScriptAPIFunction *)&Script_Maths_Sqrt, "f", sotNone },
	{ "Maths::Tan^1", (ScriptAPIFunction *)&Script_Maths_Tan, "f", sotNone },
	{ "Maths::Tanh^1", (ScriptAPIFunction *)&Script_Maths_Tanh, "f", sotNone },
	{ "Maths::get_Pi", (ScriptAPIFunction *)&Script_Maths_get_Pi, "", sotNone },
	{ "DateTime::get_Now", (ScriptAPIFunction *)&Script_DateTime_get_Now, "", sotNone },
	{ "DateTime::get_Year", (ScriptAPIFunction *)&Script_DateTime_get_Year, "", sotDateTime },
	{ "DateTime::get_Month", (ScriptAPIFunction *)&Script_DateTime_get_Month, "", sotDateTime },
	{ "DateTime::get_DayOfMonth", (ScriptAPIFunction *)&Script_DateTime_get_DayOfMonth, "", sotDateTime },
	{ "DateTime::get_Hour", (ScriptAPIFunction *)&Script_DateTime_get_Hour, "", sotDateTime },
	{ "DateTime::get_Minute", (ScriptAPIFunction *)&Script_DateTime_get_Minute, "", sotDateTime },
	{ "DateTime::get_Second", (ScriptAPIFunction *)&Script_DateTime_get_Second, "", sotDateTime },
	{ "DateTime::get_RawTime", (ScriptAPIFunction *)&Script_DateTime_get_RawTime, "", sotDateTime },
};

void addUtilsSystemScripting(AGSEngine *vm) {
	GlobalScriptState *state = vm->getScriptState();

	state->addSystemFunctionImportList(ourFunctionList, ARRAYSIZE(ourFunctionList));
}

} // End of namespace AGS
