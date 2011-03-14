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
	ESF_DECLARE(typeObject,"_constructor",0,1, new Exception(parameter[0].toString(""),0,dynamic_cast<Type*>(caller)))

	//!	[ESMF] String Exception.getMessage()
	ESMF_DECLARE(typeObject,Exception,"getMessage",0,0, String::create(self->getMessage()))

	//!	[ESMF] Number Exception.getLine()
	ESMF_DECLARE(typeObject,Exception,"getLine",0,0, Number::create(self->getLine()))

	//!	[ESMF] String Exception.getStackInfo()
	ESMF_DECLARE(typeObject,Exception,"getStackInfo",0,0, String::create(self->getStackInfo()))

	//!	[ESMF] self Exception.setMessage(String)
	ESMF_DECLARE(typeObject,Exception,"setMessage",1,1, (self->setMessage(parameter[0].toString()),self))

	//!	[ESMF] self Exception.setLine(Number)
	ESMF_DECLARE(typeObject,Exception,"setLine",1,1, (self->setLine(parameter[0].toInt()),self))

	//!	[ESMF] self Exception.setStackInfo(String)
	ESMF_DECLARE(typeObject,Exception,"setStackInfo",1,1, (self->setStackInfo(parameter[0].toString()),self))

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
	Exception * e = new Exception(msg,line,getType());
	e->setStackInfo(getStackInfo());
	return e;
}

//! ---|> [Object]
std::string Exception::toString()const {
	std::ostringstream sprinter;
	sprinter << "[#EXCEPTION \""<<msg<<"\"";
	if(getLine()>=0)
		sprinter<<" (near line "<<getLine()<<")";
	if( !stackInfo.empty() )
		sprinter << "\n"<<stackInfo;
	sprinter << "]";
	return sprinter.str();
}
