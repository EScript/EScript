// Exception.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Exception.h"

#include "../EScript.h"

#include <sstream>
using namespace EScript;

//! (static)
Type * Exception::getTypeObject(){
	// [Exception] ---|> [Object]
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}

//! initMembers
void Exception::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	declareConstant(&globals,getClassName(),typeObject);
	initPrintableName(typeObject,getClassName());
	typeObject->allowUserInheritance(true);
	
	//!	[ESMF] new Exception([String message])
	ESMF_DECLARE(typeObject,Type,"_constructor",0,1, new Exception(parameter[0].toString(""),0,self))

	//!	[ESMF] String Exception.getFilename()
	ESMF_DECLARE(typeObject,Exception,"getFilename",0,0, String::create(self->getFilename()))

	//!	[ESMF] Number Exception.getLine()
	ESMF_DECLARE(typeObject,Exception,"getLine",0,0, Number::create(self->getLine()))

	//!	[ESMF] String Exception.getMessage()
	ESMF_DECLARE(typeObject,Exception,"getMessage",0,0, String::create(self->getMessage()))

	//!	[ESMF] String Exception.getStackInfo()
	ESMF_DECLARE(typeObject,Exception,"getStackInfo",0,0, String::create(self->getStackInfo()))

	//!	[ESMF] self Exception.getFilename(String)
	ESMF_DECLARE(typeObject,Exception,"setFilename",1,1, (self->setFilename(parameter[0].toString()),self))

	//!	[ESMF] self Exception.setLine(Number)
	ESMF_DECLARE(typeObject,Exception,"setLine",1,1, (self->setLine(parameter[0].toInt()),self))

	//!	[ESMF] self Exception.setMessage(String)
	ESMF_DECLARE(typeObject,Exception,"setMessage",1,1, (self->setMessage(parameter[0].toString()),self))

	//!	[ESMF] self Exception.setStackInfo(String)
	ESMF_DECLARE(typeObject,Exception,"setStackInfo",1,1, (self->setStackInfo(parameter[0].toString()),self))

}

//! (ctor)
Exception::Exception(const std::string & _msg,int _line,Type * type):
		ExtObject(type?type:getTypeObject()),msg(_msg),line(_line),filenameId(0) {
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
	e->setFilenameId(getFilenameId());
	return e;
}

//! ---|> [Object]
std::string Exception::toString()const {
	std::ostringstream sprinter;
	sprinter << "[#"<<getTypeName();

	sprinter << " \""<<msg<<"\"";
	if(getLine()>=0 || getFilenameId()!=0){
		sprinter<<" near ";
		if(getFilenameId()!=0)
			sprinter<<"'"<<getFilename()<<"'";
		if(getLine()>=0)
			sprinter<<":"<<getLine();
		sprinter<<"";
	}

	if( !stackInfo.empty() )
		sprinter << "\n"<<stackInfo;
	sprinter << "]";
	return sprinter.str();
}
