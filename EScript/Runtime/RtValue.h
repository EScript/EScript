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
class FunctionCallContext;

//! Runtime value. Used for function return values and stack values.
class RtValue{
	public:
		enum valueType_t{
			VOID_VALUE,
			OBJECT_PTR,
			BOOL,
			UINT32,
			NUMBER,
			IDENTIFIER,
			LOCAL_STRING_IDX,
			FUNCTION_CALL_CONTEXT,
			UNDEFINED
		}valueType;
	private:
		union value_t{
			Object* value_obj;
			bool value_bool;
			uint32_t value_uint32;
			double value_number;
			StringId value_indentifier; 
			uint32_t value_localStringIndex;
			FunctionCallContext* value_fcc;
			uint64_t raw;

			value_t():raw(0){}
			value_t(const value_t &other) {raw = other.raw;}
			value_t & operator=(const value_t &other) {
				static_assert(sizeof(raw)==sizeof(value_t),"'raw' must cover the whole union.");
				raw = other.raw;
				return *this;
			}
			~value_t(){}
			void reset(){	raw = 0;	}
		}value;
		RtValue(const valueType_t type)	: valueType(type) {	}
	public:
		
		//! (factory)
		static RtValue createLocalStringIndex(const uint32_t idx){
			RtValue v(LOCAL_STRING_IDX);
			v.value.value_localStringIndex = idx;
			return v;
		}
		//! (factory)
		static RtValue createFunctionCallContext(FunctionCallContext* fcc){
			RtValue v(FUNCTION_CALL_CONTEXT);
			v.value.value_fcc = fcc;
			return v;
		}

		RtValue()						: valueType(UNDEFINED) {}
		
		template<class T ,class = typename std::enable_if<std::is_same<T,bool>::value>::type>
		RtValue(T b)					: valueType(BOOL) { value.value_bool = b;	}
		RtValue(const StringId & id)	: valueType(IDENTIFIER) { value.value_indentifier = id;	}
		RtValue(const double & v)		: valueType(NUMBER) { value.value_number = v;	}
		RtValue(const float & v)		: valueType(NUMBER) { value.value_number = v;	}
		RtValue(const int & v)			: valueType(NUMBER) { value.value_number = v;	}
		RtValue(const uint32_t & v)		: valueType(UINT32) { value.value_uint32 = v;	}
		RtValue(const std::string & s);
		RtValue(const char * s);
		RtValue(std::nullptr_t)			: valueType(VOID_VALUE) {}

		RtValue(Object * obj) {
			if(obj==nullptr){
				valueType = VOID_VALUE;
			}else{
				valueType = OBJECT_PTR;
				value.value_obj = obj;
				Object::addReference(value.value_obj);
			}
		}
		RtValue(const ObjPtr & obj) {
			if(obj==nullptr){
				valueType = VOID_VALUE;
			}else{
				valueType = OBJECT_PTR;
				value.value_obj = obj.get();
				Object::addReference(value.value_obj);
			}
		}
		RtValue(const RtValue & other) : valueType(other.valueType),value(other.value){
			if(valueType == OBJECT_PTR)
				Object::addReference(value.value_obj);
		}
		RtValue(const ObjRef & obj){
			if(obj==nullptr){
				valueType = VOID_VALUE;
			}else{
				valueType = OBJECT_PTR;
				value.value_obj = obj.get();
				Object::addReference(value.value_obj);
			}
		}
		RtValue(ObjRef && obj){
			if(obj==nullptr){
				valueType = VOID_VALUE;
			}else{
				valueType = OBJECT_PTR;
				value.value_obj = obj.detach();
			}
		}

		RtValue(RtValue && other) : valueType(other.valueType),value(other.value){
			other.valueType = UNDEFINED;
		}
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
		//! Detach the object without checking the type or changing the object's reference counter.
		Object * _detachObject(){
			valueType = RtValue::UNDEFINED;
			return value.value_obj;
		}
		Object * getObject()const				{	return valueType == OBJECT_PTR ? value.value_obj : nullptr;	}

		//! Access the value of a specific type without checking if the type is correct.
		bool _getBool()const					{	return value.value_bool;	}
		FunctionCallContext * _getFCC()const	{	return value.value_fcc;	}
		StringId _getIdentifier()const			{	return value.value_indentifier;	}
		Object * _getObject()const				{	return value.value_obj;	}
		uint32_t _getLocalStringIndex()const	{	return value.value_localStringIndex;	}
		double _getNumber()const				{	return value.value_number;	}
		uint32_t _getUInt32()const				{	return value.value_uint32;	}

		bool isFunctionCallContext()const		{	return valueType == FUNCTION_CALL_CONTEXT;	}
		bool isIdentifier()const				{	return valueType == IDENTIFIER;	}
		bool isLocalString()const				{	return valueType == LOCAL_STRING_IDX;	}
		bool isNumber()const					{	return valueType == NUMBER;	}
		bool isObject()const					{	return valueType == OBJECT_PTR;	}
		bool isUint32()const					{	return valueType == UINT32;	}
		bool isUndefined()const					{	return valueType == UNDEFINED;	}
		bool isVoid()const						{	return valueType == VOID_VALUE;	}
		
		bool toBool()const						{	return valueType == BOOL ? value.value_bool : toBool2();	}
		
	private:
		bool toBool2()const; // expensive part of toBool()
	public:
		std::string toDbgString()const;
	
		/*! Convert the value to an object; 
			\note Do not use if the type can be LOCAL_STRING_IDX or FUNCTION_CALL_CONTEXT as this can't be properly converted!*/
		Object * _toObject()const;
};

}
#endif // ES_VALUE_H
