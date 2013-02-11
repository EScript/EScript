// Number.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef NUMBER_H
#define NUMBER_H

#include "../Type.h"
#include <string>

#include <limits>

// assure M_PI is defined in VC (necessary for VC)
#define _USE_MATH_DEFINES
#include <cmath>

namespace EScript {

//! [Number] ---|> [Object]
class Number : public Object {
		ES_PROVIDES_TYPE_NAME(Number)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);

		// ---
		static Number * create(double value);
		static void release(Number * n);

		// ---
		/**
		 * Test if the two parameters are essentially equal as defined in
		 * the given reference by Knuth.
		 *
		 * @param u First floating point parameter.
		 * @param v Second floating point parameter.
		 * @return @c true if both floating point values are essentially
		 * equal, @c false otherwise.
		 * @see Donald E. Knuth: The art of computer programming.
		 * Volume II: Seminumerical algorithms. Addison-Wesley, 1969.
		 * Page 128, Equation (24).
		 */
		inline static bool matches(const float u, const float v) {
			return std::abs(v - u) <= std::numeric_limits<float>::epsilon() * std::min(std::abs(u), std::abs(v));
		}

		explicit Number(double value);
		virtual ~Number()									{}

		double & operator*()								{	return value;	}
		double operator*()const								{	return value;	}

		/**
		 * Convert the number to a string configurable string representation.
		 *
		 * @param precision Number of digits after the decimal point.
		 * @param scientific Switch for scientific or fixed-point notation.
		 * @param width Number of digits for padding.
		 * @param fill Character to use for padding.
		 * @return Formatted string representation of the number.
		 */
		std::string format(std::streamsize precision = 3, bool scientific = true, std::streamsize width = 0, char fill = '0') const;

		//! Floating point symmetric modulo operation
		double modulo(const double m)const;

		double getValue()const								{	return value;	}
		void setValue(double _value)						{	value = _value;	}

		//! ---|> [Object]
		virtual Object * clone()const						{	return create(value);	}

		virtual std::string toString()const;
		virtual double toDouble()const						{	return value;	}
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr & o);
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_NUMBER;	}

	private:
		double value;
};

template<>
double convertTo<double>(Runtime& rt,ObjPtr src);
template<>
inline float convertTo<float>(Runtime& rt,ObjPtr src)		{	return static_cast<float>(convertTo<double>(rt,src));	}
template<>
inline int64_t convertTo<int64_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int64_t>(convertTo<double>(rt,src));	}
template<>
inline uint64_t convertTo<uint64_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint64_t>(convertTo<double>(rt,src));	}
template<>
inline int32_t convertTo<int32_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int32_t>(convertTo<double>(rt,src));	}
template<>
inline uint32_t convertTo<uint32_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint32_t>(convertTo<double>(rt,src));	}
template<>
inline int16_t convertTo<int16_t>(Runtime& rt,ObjPtr src)	{	return static_cast<int16_t>(convertTo<double>(rt,src));	}
template<>
inline uint16_t convertTo<uint16_t>(Runtime& rt,ObjPtr src)	{	return static_cast<uint16_t>(convertTo<double>(rt,src));	}

}

#endif // NUMBER_H
