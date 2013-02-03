// Attribute.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Attribute_H
#define ES_Attribute_H

#include "ObjRef.h"
#include "StringId.h"
#include <cstdint>

namespace EScript {

//! Internal representation of an object's attribute
class Attribute{
	public:
		typedef uint8_t flag_t;
		static const flag_t NORMAL_ATTRIBUTE = 0;
		static const flag_t CONST_BIT = (1<<0);	// 0...normal	1...const
		static const flag_t PRIVATE_BIT = (1<<1);	// 0...public	1...private
		static const flag_t TYPE_ATTR_BIT = (1<<2);	// 0...objAttr	1...typeAttr
		static const flag_t INIT_BIT = (1<<3);		// 0...normal	1...init
		static const flag_t REFERENCE_BIT = (1<<4);	// 0...normal	1...reference    //! \todo Unused! Check if still useful.
		static const flag_t OVERRIDE_BIT = (1<<5);	// 0...normal	1...override

		static const flag_t ASSIGNMENT_RELEVANT_BITS = CONST_BIT|PRIVATE_BIT|REFERENCE_BIT;

	private:
		ObjRef value;
		flag_t properties;
	public:
		explicit Attribute(flag_t _properties = NORMAL_ATTRIBUTE):properties(_properties) {}
		/*implicit*/ Attribute(const ObjPtr & _value,flag_t  _properties = NORMAL_ATTRIBUTE):value(_value.get()),properties(_properties) {}
		/*implicit*/ Attribute(const ObjRef & _value,flag_t  _properties = NORMAL_ATTRIBUTE):value(_value.get()),properties(_properties) {}
		/*implicit*/ Attribute(Object * _value,flag_t  _properties = NORMAL_ATTRIBUTE):value(_value),properties(_properties) {}
		/*implicit*/ Attribute(const Attribute & e):value(e.value),properties(e.properties) {}

		bool getProperty(flag_t f)const	{	return (properties&f)>0;	}
		flag_t getProperties()const		{	return properties;	}

		Object * getValue()const		{	return value.get();	}
		bool isConst()const				{	return properties&CONST_BIT;	}
		bool isInitializable()const		{	return properties&INIT_BIT;	}
		bool isNull()const				{	return value.isNull();	}
		bool isNotNull()const			{	return value.isNotNull();	}
		bool isObjAttribute()const		{	return !(properties&TYPE_ATTR_BIT);	}
		bool isTypeAttribute()const		{	return properties&TYPE_ATTR_BIT;	}
		bool isPrivate()const			{	return properties&PRIVATE_BIT;	}
		bool isReference()const			{	return properties&REFERENCE_BIT;	} //! \todo Unused! Check if still useful.
		bool isOverriding()const		{	return properties&OVERRIDE_BIT;	}

		void setValue(Object * v)		{	value = v;	}
		void set(Object * v,flag_t f)	{	value = v, properties = f;	}
		Attribute & operator=(const Attribute & e){
			set(e.value.get(), e.properties);
			return *this;
		}
};
}
#endif // ES_Attribute_H
