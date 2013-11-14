// UserFunction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunction.h"
#include "../../Basics.h"
#include <sstream>

namespace EScript{

//! (static)
Type * UserFunction::getTypeObject(){
	static Type * typeObject = new Type(ExtObject::getTypeObject()); // ---|> ExtObject
	return typeObject;
}

//! (static) initMembers
void UserFunction::init(EScript::Namespace & globals) {
	// [UserFunction] ---|> [ExtObject] ---|> [Object]
	Type * typeObject = getTypeObject();
	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] String UserFunction.getFilename()
	ES_MFUN(typeObject,UserFunction,"getFilename",0,0,thisObj->getCode().getFilename())

	//! [ESMF] String UserFunction.getCode()
	ES_MFUN(typeObject,UserFunction,"getCode",0,0,thisObj->getCode().getCodeString())

	//! [ESMF] Number|false UserFunction.getMaxParamCount()
	ES_MFUNCTION(typeObject,UserFunction,"getMaxParamCount",0,0,{
		if(thisObj->getMaxParamCount()<0 )
			return false;
		return thisObj->getMaxParamCount();
	})
	//! [ESMF] Number|false UserFunction.getMultiParam()
	ES_MFUNCTION(typeObject,UserFunction,"getMultiParam",0,0,{
		if(thisObj->getMultiParam()<0 )
			return false;
		return thisObj->getMultiParam();
	})

	//! [ESMF] Number UserFunction.getMinParamCount()
	ES_MFUN(typeObject,UserFunction,"getMinParamCount",0,0, thisObj->getMinParamCount())

	//! [ESMF] Number UserFunction.getParamCount()
	ES_MFUN(typeObject,UserFunction,"getParamCount",0,0, static_cast<uint32_t>(thisObj->getParamCount()))

	//! [ESMF] String UserFunction._asm()
	ES_MFUN(typeObject,UserFunction,"_asm",0,0, thisObj->getInstructionBlock().toString())

}

//! (ctor)
UserFunction::UserFunction(const UserFunction & other) :
		ExtObject(other),codeFragment(other.codeFragment),line(other.line),
		paramCount(other.paramCount),minParamValueCount(other.minParamValueCount),maxParamValueCount(other.maxParamValueCount),
		multiParam(other.multiParam),instructions(other.instructions),
		staticData(other.staticData){
}

//! (ctor)
UserFunction::UserFunction() :
		ExtObject(getTypeObject()),line(-1),paramCount(0),
		minParamValueCount(0),maxParamValueCount(0),multiParam(-1) {
	//ctor
}

//! ---|> Object
std::string UserFunction::toDbgString()const{
	std::ostringstream os;
	// UserFunction is created from "eval" or "load" and has no own "fn(...)"-part
	if(getLine()<0){
		os << toString()<<"_("<<codeFragment.getFilename()<<")";
	}else{
		size_t end = codeFragment.getFullCode().find('{',codeFragment.getStartPos());
		os << codeFragment.getFullCode().substr(codeFragment.getStartPos(),end-codeFragment.getStartPos())
		<< "{...}_("<<codeFragment.getFilename()<<":"<< getLine() <<")";
	}

	return os.str();
}
}

