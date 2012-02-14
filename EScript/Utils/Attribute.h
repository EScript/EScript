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
		static const flag_t NORMAL_ATTRIBUTE = 0;
		static const flag_t CONST_BIT = (1<<0); 	// 0...normal 	1...const
		static const flag_t PRIVATE_BIT = (1<<1); 	// 0...public	1...private
		static const flag_t TYPE_ATTR_BIT = (1<<2);	// 0...objAttr	1...typeAttr
		static const flag_t INIT_BIT = (1<<3);		// 0...normal	1...init
		static const flag_t REFERENCE_BIT = (1<<4);	// 0...normal	1...reference


		static const flag_t ASSIGNMENT_RELEVANT_BITS = CONST_BIT|PRIVATE_BIT|REFERENCE_BIT;

	private:
		ObjRef value;
		flag_t flags;
	public:
		explicit Attribute(flag_t _flags = NORMAL_ATTRIBUTE):flags(_flags) {}
		/*implicit*/ Attribute(const ObjPtr & _value,flag_t  _flags = NORMAL_ATTRIBUTE):value(_value.get()),flags(_flags) {}
		/*implicit*/ Attribute(Object * _value,flag_t  _flags = NORMAL_ATTRIBUTE):value(_value),flags(_flags) {}
		/*implicit*/ Attribute(const Attribute & e):value(e.value),flags(e.flags) {}

		bool getFlag(flag_t f)const			{	return (flags&f)>0;	}
		flag_t getFlags()const				{	return flags;	}

		Object * getValue()const 			{	return value.get();	}
		bool isConst()const					{	return flags&CONST_BIT;	}
		bool isInitializable()const 		{	return flags&INIT_BIT;	}
		bool isNull()const 					{	return value.isNull();	}
		bool isObjAttribute()const 			{	return !getFlag(TYPE_ATTR_BIT);	}
		bool isTypeAttribute()const 		{	return getFlag(TYPE_ATTR_BIT);	}
		bool isPrivate()const				{	return flags&PRIVATE_BIT;	}
		bool isReference()const				{	return flags&REFERENCE_BIT;	}

		void setValue(Object * v)			{	value=v;	}
		void set(Object * v,flag_t f)		{	value=v, flags=f;	}
		Attribute & operator=(const Attribute & e){
			set(e.value.get(), e.flags);
			return *this;
		}
};
}
#endif // ES_Attribute_H
