// Exception.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Exception.h"

#include "../EScript.h"

#include <sstream>
using namespace EScript;

Type* Exception::typeObject=NULL;

//! initMembers
void Exception::init(EScript::Namespace & globals) {
	// Exception ---|> [ExtObject] ---|> [Object]
	typeObject=new Type(ExtObject::getTypeObject());
	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] new Exception([String message])
	ESF_DECLARE(typeObject,"_constructor",0,1,
				new Exception(parameter[0].toString(""),0,dynamic_cast<Type*>(caller)))
}

//! (ctor)
Exception::Exception(const std::string & _msg,int _line,Type * type):
		ExtObject(type?type:typeObject),msg(_msg),line(_line) {
	//ctor
}

//! (dtor)
Exception::~Exception() {
	//dtor
}

//! ---|> [Object]
Object *  Exception::clone()const {
	return new Exception(msg,line,getType());
}

//! ---|> [Object]
std::string Exception::toString()const {
	std::ostringstream sprinter;
	sprinter << "[#EXCEPTION \""<<msg<<"\" ("<<getFilename();
	if(getLine()>=0)
		sprinter<<" near line "<<getLine();
	sprinter << ")]";
	return sprinter.str();
}
