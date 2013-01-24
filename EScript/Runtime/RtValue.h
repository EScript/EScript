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
	RtValue(const StringId & id)		: valueType(IDENTIFIER) { value.value_indentifier = id.getValue();	}
	RtValue(const double & v)		: valueType(NUMBER) { value.value_number = v;	}
	RtValue(const float & v)			: valueType(NUMBER) { value.value_number = v;	}
	RtValue(const uint32_t & v)		: valueType(UINT32) { value.value_uint32 = v;	}

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
	bool isObject()const					{	return valueType == OBJECT_PTR;	}
	Object * getObject()const				{	return valueType == OBJECT_PTR ? value.value_obj : nullptr;	}

	bool isUndefined()const					{	return valueType == UNDEFINED;	}
	
	inline Object * operator->()const			{	return getObject(); }


// --------------------------
	bool toBool(const bool defaultValue = false)const;
	double toDouble(const double defaultValue = 0.0)const;
	float toFloat(const float defaultValue = 0.0f)const;
	int toInt(const int defaultValue = 0)const;
	unsigned int toUInt(const unsigned int defaultValue = 0)const;
	std::string toString()const;
	std::string toString(const char * defaultValue)const;
	
// --------------

	template<class objType>	objType * detachObject(){
		if(isObject()){
			objType * obj = dynamic_cast<objType*>(value.value_obj);
			if(obj){
				Object::decreaseReference(obj);
				valueType = UNDEFINED;
				return obj;
			}
		}
		return nullptr;
	}
	template<class T> T* toType()const{
		return dynamic_cast<T*>(getObject());
	}

	Object * detachObject(){
		if(isObject()){
			Object::decreaseReference(value.value_obj);
			valueType = UNDEFINED;
			return value.value_obj;
		}
		return nullptr;
	}

	Object * toObject()const;
};

inline RtValue rtValue(const float v)		{	return RtValue(v);	}
inline RtValue rtValue(const double v)		{	return RtValue(v);	}
inline RtValue rtValue(const bool v)		{	return RtValue(v);	}
inline RtValue rtValue(Object * v)			{	return RtValue(v);	}
inline RtValue rtValue(const ObjRef & v)	{	return RtValue(v);	}
inline RtValue rtValue(ObjRef && v)			{	return RtValue(v);	}
RtValue rtValue(const std::string & s);

}
#endif // ES_VALUE_H
