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
#include <stdexcept>

namespace EScript{
	
RtValue::RtValue(const std::string & s) : 
		valueType(OBJECT_PTR){ 
	value.value_obj = String::create(s);
	Object::addReference(value.value_obj);
}
RtValue::RtValue(const char * s) : 
		valueType(OBJECT_PTR){ 
	value.value_obj = String::create(s);
	Object::addReference(value.value_obj);
}

std::string RtValue::toDbgString()const{
	switch(valueType){
		case VOID_VALUE:
			return "void";
		case BOOL:
			return value.value_bool ? "true" : "false";
		case OBJECT_PTR:
			return value.value_obj->toDbgString();
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
			return value.value_indentifier.toString();
		case LOCAL_STRING_IDX:{
			std::ostringstream s;
			s<<"[Local string #"<< value.value_localStringIndex <<"]";
			return s.str();
		}
		case UNDEFINED:{
			return "[UndefinedRTValue]";
		}
		case FUNCTION_CALL_CONTEXT:{
			return "[FCC]";
		}
		default:
			return "";
	}
}

bool RtValue::toBool2()const{
	if(isObject()){
		return value.value_obj->toBool();
	}else if(isVoid() || isUndefined()){
		return false;
	}else{
		return true;
	}
}

Object * RtValue::_toObject()const{
	switch(valueType){
		case VOID_VALUE:
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
		case UNDEFINED:
		case FUNCTION_CALL_CONTEXT:
		default:
			throw std::logic_error("RtValue can't be converted to into an object:"+toDbgString());
			return nullptr;
	}
}

}
