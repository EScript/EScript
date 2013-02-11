// UserFunction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunction.h"
#include "../../EScript.h"
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
	Type * t = getTypeObject();
	declareConstant(&globals,getClassName(),t);

	//! [ESMF] String UserFunction.getFilename()
	ESMF_DECLARE(t,UserFunction,"getFilename",0,0,self->getCode().getFilename())

	//! [ESMF] String UserFunction.getCode()
	ESMF_DECLARE(t,UserFunction,"getCode",0,0,self->getCode().getCodeString())

	//! [ESMF] Number|false UserFunction.getMaxParamCount()
	ES_MFUNCTION_DECLARE(t,UserFunction,"getMaxParamCount",0,0,{
		if(self->getMaxParamCount()<0 )
			return false;
		return self->getMaxParamCount();
	})
	//! [ESMF] Number|false UserFunction.getMultiParam()
	ES_MFUNCTION_DECLARE(t,UserFunction,"getMultiParam",0,0,{
		if(self->getMultiParam()<0 )
			return false;
		return self->getMultiParam();
	})

	//! [ESMF] Number UserFunction.getMinParamCount()
	ESMF_DECLARE(t,UserFunction,"getMinParamCount",0,0, self->getMinParamCount())

	//! [ESMF] Number UserFunction.getParamCount()
	ESMF_DECLARE(t,UserFunction,"getParamCount",0,0, static_cast<uint32_t>(self->getParamCount()))

	//! [ESMF] String UserFunction._asm()
	ESMF_DECLARE(t,UserFunction,"_asm",0,0, self->getInstructionBlock().toString())

}

//! (ctor)
UserFunction::UserFunction(const UserFunction & other) :
		ExtObject(other),codeFragment(other.codeFragment),line(other.line),
		paramCount(other.paramCount),minParamValueCount(other.minParamValueCount),maxParamValueCount(other.maxParamValueCount),
		multiParam(other.multiParam),instructions(other.instructions){
}

//! (ctor)
UserFunction::UserFunction() :
		ExtObject(getTypeObject()),line(-1),paramCount(0),minParamValueCount(0),maxParamValueCount(0),multiParam(-1) {
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

