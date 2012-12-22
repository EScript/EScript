// NumberRef.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "NumberRef.h"
#include "../../EScript.h"

using namespace EScript;
//---

//---
//! (static)
Type * NumberRef::getTypeObject(){
	// [NumberRef] ---|> [Number]
	static Type * typeObject = new Type(Number::getTypeObject());
	return typeObject;
}

//! initMembers
void NumberRef::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	declareConstant(&globals,getClassName(),typeObject);
	initPrintableName(typeObject,getClassName());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

	declareConstant(&globals,getClassName(),typeObject);
}

//---

//! (ctor)
NumberRef::NumberRef(double & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_DOUBLE) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(float & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_FLOAT) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(int & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_INT) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(unsigned int & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_UINT) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(char & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_CHAR) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(unsigned char & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_UCHAR) {
	valuePtr= &_valueRef;
	//ctor
}

//! (ctor)
NumberRef::NumberRef(long & _valueRef,Type * type):
		Number(0,type?type:getTypeObject(),true),valueType(VT_LONG) {
	valuePtr= &_valueRef;
	//ctor
}

//! (dtor)
NumberRef::~NumberRef() {
	valuePtr = nullptr;
	//dtor
}

//! ---|> Number
double NumberRef::getValue()const {
	switch (valueType) {
	case VT_DOUBLE: {
		return *reinterpret_cast<double *>(valuePtr);
	}
	case VT_INT: {
		return static_cast<double>(*reinterpret_cast<int *>(valuePtr));
	}
	case VT_UINT: {
		return static_cast<double>(*reinterpret_cast<unsigned int *>(valuePtr));
	}
	case VT_CHAR: {
		return static_cast<double>(*reinterpret_cast<char *>(valuePtr));
	}
	case VT_UCHAR: {
		return static_cast<double>(*reinterpret_cast<unsigned char *>(valuePtr));
	}
	case VT_LONG: {
		return static_cast<double>(*reinterpret_cast<long *>(valuePtr));
	}
	case VT_FLOAT: {
		return static_cast<double>(*reinterpret_cast<float *>(valuePtr));
	}
	default:
		return 0;
	}
}

//! ---o
void NumberRef::_assignValue(ObjPtr newValue){
	setValue(newValue.toDouble());
}

//! ---|> Number
void NumberRef::setValue(double _value) {

	switch (valueType) {
	case VT_DOUBLE: {
		*reinterpret_cast<double *>(valuePtr)=_value;
		break;
	}
	case VT_INT: {
		*reinterpret_cast<int *>(valuePtr)=static_cast<int>(_value);
		break;
	}
	case VT_UINT: {
		*reinterpret_cast<unsigned int *>(valuePtr)=static_cast<unsigned int>(_value);
		break;
	}
	case VT_CHAR: {
		*reinterpret_cast<char  *>(valuePtr)=static_cast<char>(_value);
		break;
	}
	case VT_UCHAR: {
		*reinterpret_cast<unsigned char *>(valuePtr)=static_cast<unsigned char>(_value);
		break;
	}
	case VT_LONG: {
		*reinterpret_cast<long *>(valuePtr)=static_cast<long>(_value);
		break;
	}
	case VT_FLOAT: {
		*reinterpret_cast<float *>(valuePtr)=static_cast<float>(_value);
		break;
	}
	default:
		break;
	}
}

//! ---|> [Object]
Object * NumberRef::getRefOrCopy() {
	return new Number(getValue());
}
