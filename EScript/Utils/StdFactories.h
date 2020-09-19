// StdFactories.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_STD_FACTORIES_H_INCLUDED
#define ES_STD_FACTORIES_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <type_traits>
#include "../Runtime/RtValue.h"

namespace EScript {

class Array;
class Bool;
class Number;
class Object;
class String;
class StringData;
class Void;

/*! Factories for types that are not results of unwanted implicit type conversions. */
ESCRIPTAPI Array* create(const std::vector<Object*>&);
ESCRIPTAPI String* create(const StringData &);
ESCRIPTAPI String* create(const std::string &);
ESCRIPTAPI String* create(const char*);
ESCRIPTAPI Void* create(std::nullptr_t);
ESCRIPTAPI Object* create(const ObjRef& obj);

/*!	For primitive types that can be the result of an implicit type conversion,
	a template function is used that requires the exact type.
	This allows detecting errors if EScript::create(SomeType*) is
	not specified for "SomeType*" (or the corresponding include is missing) and
	the implicit cast would then result in creating an object of type Bool.	*/
namespace _Internals{
ESCRIPTAPI Number * createNumber(double v);
ESCRIPTAPI Number * createNumber(float v);
ESCRIPTAPI Number * createNumber(int v);
ESCRIPTAPI Number * createNumber(uint32_t v);
ESCRIPTAPI Bool * createBool(bool v);
}

template<class T ,class = typename std::enable_if<std::is_same<T,bool>::value>::type>
Bool * create(T value)			{	return _Internals::createBool(value);	}

template<class T ,class = typename std::enable_if<std::is_same<T,double>::value ||
		std::is_same<T,float>::value || std::is_same<T,int>::value ||
		std::is_same<T,uint32_t>::value>::type>
Number * create(T value)		{	return _Internals::createNumber(value);	}



/*! EScript::value(someValue) returns a RtValue wrapping 'someValue'. If 'someValue' can not be represented as
	RtValue directly, a corresponding EScript object is created by calling EScript::create(someValue).
	\note All specific create functions must be present before this file is included in order to be found by the
		compiler. \see http://gcc.gnu.org/gcc-4.7/porting_to.html (Name lookup changes)
	*/
template<typename source_t>
inline RtValue value(source_t obj,typename std::enable_if<std::is_convertible<source_t, RtValue>::value>::type * = nullptr){
	return RtValue(obj);
}

template<typename source_t>
inline RtValue value(source_t obj,typename std::enable_if<!std::is_convertible<source_t, RtValue>::value>::type * = nullptr){
	return EScript::create(obj);
}

}

#endif // ES_STD_FACTORIES_H_INCLUDED
