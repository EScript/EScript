// RtValue.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "RtValue.h"
#include "../Objects/Object.h"
#include "../Objects/Identifier.h"
#include "../Objects/Values/Bool.h"
#include "../Objects/Values/String.h"
#include "../Objects/Values/Number.h"
#include "../Objects/Values/String.h"
#include "../Objects/Values/Void.h"

namespace EScript{
std::string RtValue::toString()const{
	switch(valueType){
		case VOID:
			return "void";
		case BOOL:
			return value.value_bool ? "true" : "false";
		case OBJECT_PTR:
			return value.value_obj->toString();
		case UINT32:{
			std::ostringstream s;
			s<<value.value_uint32;
			return s.str();
		}
		case NUMBER:{
			std::ostringstream s;
			s<<value.value_number;
			return s.str();
		}
		case IDENTIFIER:
			return StringId::toString(value.value_indentifier);
		case LOCAL_STRING_IDX:{
			std::ostringstream s;
			s<<"[Local string #"<< value.value_localStringIndex <<"]";
			return s.str();
		}
		case UNDEFINED:
		default:
			return "";
	}
}
std::string RtValue::toString(const char * defaultValue)const{
	switch(valueType){
		case VOID:
			return "void";
		case BOOL:
			return value.value_bool ? "true" : "false";
		case OBJECT_PTR:
			return value.value_obj->toString();
		case UINT32:{
			std::ostringstream s;
			s<<value.value_uint32;
			return s.str();
		}
		case NUMBER:{
			std::ostringstream s;
			s<<value.value_number;
			return s.str();
		}
		case IDENTIFIER:
			return StringId::toString(value.value_indentifier);
		case LOCAL_STRING_IDX:
			return "Local string";
		case UNDEFINED:
		default:
			return defaultValue;
	}
}
bool RtValue::toBool(const bool defaultValue)const{
	switch(valueType){
		case VOID:
			return false;
		case BOOL:
			return value.value_bool;
		case OBJECT_PTR:
			return value.value_obj->toBool();
		case UNDEFINED:
			return defaultValue;
		case LOCAL_STRING_IDX:
		case UINT32:
		case NUMBER:
		case IDENTIFIER:
		default:
			return true;
	}
}

double RtValue::toDouble(const double defaultValue)const{
	if(valueType==NUMBER){
		return value.value_number;
	}else if(valueType==OBJECT_PTR){
		return value.value_obj->toDouble();
	}else if(valueType==UINT32){
		return value.value_uint32;
	}else if(valueType==UNDEFINED){
		return defaultValue;
	}else return 0.0;
}
float RtValue::toFloat(const float defaultValue)const{
	if(valueType==NUMBER){
		return static_cast<float>(value.value_number);
	}else if(valueType==OBJECT_PTR){
		return value.value_obj->toFloat();
	}else if(valueType==UINT32){
		return static_cast<float>(value.value_uint32);
	}else if(valueType==UNDEFINED){
		return defaultValue;
	}else return 0.0;
}
int RtValue::toInt(const int defaultValue)const{
	if(valueType==NUMBER){
		return static_cast<int>(value.value_number);
	}else if(valueType==OBJECT_PTR){
		return value.value_obj->toInt();
	}else if(valueType==UINT32){
		return  static_cast<int>(value.value_uint32);
	}else if(valueType==UNDEFINED){
		return defaultValue;
	}else return 0;
}
unsigned int RtValue::toUInt(const unsigned int defaultValue)const{
	if(valueType==NUMBER){
		return static_cast<unsigned int>(value.value_number);
	}else if(valueType==OBJECT_PTR){
		return value.value_obj->toUInt();
	}else if(valueType==UINT32){
		return  static_cast<unsigned int>(value.value_uint32);
	}else if(valueType==UNDEFINED){
		return defaultValue;
	}else return 0;
}

Object * RtValue::toObject()const{
	switch(valueType){
		case VOID:
			return Void::get();
		case BOOL:
			return Bool::create(value.value_bool);
		case OBJECT_PTR:
			return value.value_obj;
		case UINT32:
			return Number::create(value.value_uint32);
		case NUMBER:
			return Number::create(value.value_number);
		case IDENTIFIER:
			return Identifier::create(StringId( value.value_indentifier));
		case LOCAL_STRING_IDX:
			return String::create("[Local string]");
		case UNDEFINED:
		default:
			return nullptr;
	}
}


RtValue rtValue(const std::string & s){
	return rtValue(String::create(s));

}
}
