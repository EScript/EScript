// Exception.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Exception.h"

#include "../Basics.h"

#include <sstream>
namespace EScript{

//! (static)
Type * Exception::getTypeObject(){
	static Type * typeObject = new Type(ExtObject::getTypeObject()); // ---|> ExtObject
	return typeObject;
}

//! initMembers
void Exception::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());
	typeObject->allowUserInheritance(true);

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] new Exception([String message])
	ES_CTOR(typeObject,0,1, new Exception(parameter[0].toString(""),0,thisType))

	//!	[ESMF] String Exception.getFilename()
	ES_MFUN(typeObject,Exception,"getFilename",0,0, thisObj->getFilename())

	//!	[ESMF] Number Exception.getLine()
	ES_MFUN(typeObject,Exception,"getLine",0,0, thisObj->getLine())

	//!	[ESMF] String Exception.getMessage()
	ES_MFUN(typeObject,Exception,"getMessage",0,0, thisObj->getMessage())

	//!	[ESMF] String Exception.getStackInfo()
	ES_MFUN(typeObject,Exception,"getStackInfo",0,0, thisObj->getStackInfo())

	//!	[ESMF] thisObj Exception.getFilename(String)
	ES_MFUN(typeObject,Exception,"setFilename",1,1, (thisObj->setFilename(parameter[0].toString()),thisEObj))

	//!	[ESMF] thisObj Exception.setLine(Number)
	ES_MFUN(typeObject,Exception,"setLine",1,1, (thisObj->setLine(parameter[0].to<int>(rt)),thisEObj))

	//!	[ESMF] thisObj Exception.setMessage(String)
	ES_MFUN(typeObject,Exception,"setMessage",1,1, (thisObj->setMessage(parameter[0].toString()),thisEObj))

	//!	[ESMF] thisObj Exception.setStackInfo(String)
	ES_MFUN(typeObject,Exception,"setStackInfo",1,1, (thisObj->setStackInfo(parameter[0].toString()),thisEObj))

}

//! (ctor)
Exception::Exception(const std::string & _msg,int _line,Type * type):
		ExtObject(type?type:getTypeObject()),msg(_msg),line(_line),filenameId(0) {
	//ctor
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
	if(getLine()>=0 || !getFilenameId().empty()){
		sprinter<<" near ";
		if(!getFilenameId().empty())
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
}
