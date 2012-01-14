// Attribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Attribute_H
#define ES_Attribute_H

#include "ObjRef.h"
#include <stdint.h>

namespace EScript {

/*! Internal representation of an object's attribute */
class Attribute{
	public:
		typedef uint8_t flag_t;
		static const flag_t CONST = 1; // else VARIABLE
		static const flag_t PRIVATE = 2; // else PUBLIC
		static const flag_t TYPE_ATTR = 4; // else OBJ_ATTR
	private:
		ObjRef value;
		flag_t flags;
	public:
		Attribute(flag_t _flags=0):flags(_flags) {}
		Attribute(Object * _value,flag_t  _flags):value(_value),flags(_flags) {}
		Attribute(const Attribute & e):value(e.value),flags(e.flags) {}

		bool getFlag(flag_t f)const			{	return (flags&f)>0;	}
		flag_t getFlags()const				{	return flags;	}

		Object * getValue()const 			{	return value.get();	}
		bool isObjAttribute()const 			{	return !getFlag(TYPE_ATTR);	}
		bool isTypeAttribute()const 		{	return getFlag(TYPE_ATTR);	}
		void setValue(Object * v)			{	value=v;	}
		void set(Object * v,flag_t f)		{	value=v, flags=f;	}
		Attribute & operator=(const Attribute & e){
			set(e.value.get(), e.flags);
			return *this;
		}
};
}
#endif // ES_Attribute_H
