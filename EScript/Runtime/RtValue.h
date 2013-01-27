// RtValue.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_VALUE_H
#define ES_VALUE_H

#include <cstddef>
#include <string>
#include <iostream>
#include <sstream>
#include "../Objects/Object.h"
#include "../Utils/ObjRef.h"
#include "../Utils/StringId.h"

namespace EScript {

struct RtValue{
	enum valueType_t{
		VOID,
		OBJECT_PTR,
		BOOL,
		UINT32,
		NUMBER,
		IDENTIFIER,
		LOCAL_STRING_IDX,
		UNDEFINED
//				UINT32_PAIR // \todo coming with c++11
	}valueType;

	union value_t{
		Object * value_obj;
		bool value_bool;
		uint32_t value_uint32;
		double value_number;
		uint32_t value_indentifier; // \todo c++0x unrestricted union allows StringId
		uint32_t value_localStringIndex;
		uint64_t raw;

		value_t():raw(0){}
		value_t(const value_t &other) {raw = other.raw;}
		value_t & operator=(const value_t &other) { // dangeraous!
			raw = other.raw; // static assert max size
			return *this;
		}
		~value_t(){}
		void reset(){	raw = 0;	}
	}value;

	//! (factory)
	static RtValue createLocalStringIndex(const uint32_t idx){
		RtValue v(LOCAL_STRING_IDX);
		v.value.value_localStringIndex = idx;
		return v;
	}

	RtValue()						: valueType(UNDEFINED) {}
	RtValue(const bool b)			: valueType(BOOL) { value.value_bool = b;	}
	RtValue(const StringId & id)	: valueType(IDENTIFIER) { value.value_indentifier = id.getValue();	}
	RtValue(const double & v)		: valueType(NUMBER) { value.value_number = v;	}
	RtValue(const float & v)		: valueType(NUMBER) { value.value_number = v;	}
	RtValue(const int & v)			: valueType(NUMBER) { value.value_number = v;	}
	RtValue(const uint32_t & v)		: valueType(UINT32) { value.value_uint32 = v;	}
	RtValue(const std::string & s);
	RtValue(std::nullptr_t)			: valueType(VOID) {}

	RtValue(Object * obj) {
		if(obj==nullptr){
			valueType = VOID;
		}else{
			valueType = OBJECT_PTR;
			value.value_obj = obj;
			Object::addReference(value.value_obj);
		}
	}
	RtValue(const RtValue & other) : valueType(other.valueType),value(other.value){
		if(valueType == OBJECT_PTR)
			Object::addReference(value.value_obj);
	}
	RtValue(const ObjRef & obj){
		if(obj==nullptr){
			valueType = VOID;
		}else{
			valueType = OBJECT_PTR;
			value.value_obj = obj.get();
			Object::addReference(value.value_obj);
		}
	}
	RtValue(ObjRef && obj){
		if(obj==nullptr){
			valueType = VOID;
		}else{
			valueType = OBJECT_PTR;
			value.value_obj = obj.detach();
		}
	}

	RtValue(RtValue && other) : valueType(other.valueType),value(other.value){
		other.valueType = UNDEFINED;
	}
private:
	RtValue(const valueType_t type)	: valueType(type) {	}
public:

	~RtValue(){
		if(valueType == OBJECT_PTR)
			Object::removeReference(value.value_obj);
	}
	RtValue & operator=(const RtValue & other){
		if(other.valueType == OBJECT_PTR){ // new value is object
			Object::addReference(other.value.value_obj);
		}
		if(valueType==OBJECT_PTR){ // old value is object
			Object::removeReference(value.value_obj);
		}
		valueType = other.valueType;
		value = other.value;
		return * this;
	}
	RtValue & operator=(RtValue && other){
		if(valueType==OBJECT_PTR)
			Object::removeReference(value.value_obj);

		valueType = other.valueType;
		value = other.value;
		other.valueType = UNDEFINED;
		return * this;
	}
	Object * getObject()const				{	return valueType == OBJECT_PTR ? value.value_obj : nullptr;	}

	bool isObject()const					{	return valueType == OBJECT_PTR;	}
	bool isUndefined()const					{	return valueType == UNDEFINED;	}
	bool isVoid()const						{	return valueType == VOID;	}
	bool isUint32()const					{	return valueType == UINT32;	}
	bool isNumber()const					{	return valueType == NUMBER;	}
	bool isIdentifier()const				{	return valueType == IDENTIFIER;	}
	bool isLocalString()const				{	return valueType == LOCAL_STRING_IDX;	}
// --------------------------
	bool toBool()const{
		return valueType == BOOL ? value.value_bool : toBool2();
	}
private:
	bool toBool2()const; // expensive part of toBool()
public:
	std::string toDbgString()const;

	
// --------------

	/*! Convert the value to an object; 
		\note Do not use if the type can be LOCAL_STRING_IDX as this can't be properly converted!*/
	Object * _toObject()const;
};

inline RtValue rtValue(const float v)		{	return RtValue(v);	}
inline RtValue rtValue(const double v)		{	return RtValue(v);	}
inline RtValue rtValue(const bool v)		{	return RtValue(v);	}
inline RtValue rtValue(Object * v)			{	return RtValue(v);	}
inline RtValue rtValue(const ObjRef & v)	{	return RtValue(v);	}
inline RtValue rtValue(ObjRef && v)			{	return RtValue(v);	}
inline RtValue rtValue(const std::string&v)	{	return RtValue(v);	}

}
#endif // ES_VALUE_H
