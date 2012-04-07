// UserFunction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunction.h"
#include "../AST/BlockStatement.h"
#include "../../EScript.h"
#include "../../Parser/CompilerContext.h"
#include "../../Parser/Compiler.h"
#include <sstream>

using namespace EScript;


//! (static)
Type * UserFunction::getTypeObject()	{
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}

//! (static) initMembers
void UserFunction::init(EScript::Namespace & globals) {
	// [UserFunction] ---|> [ExtObject] ---|> [Object]
	Type * t=getTypeObject();
	declareConstant(&globals,getClassName(),t);

	//! [ESMF] String UserFunction.getFilename()
	ESMF_DECLARE(t,UserFunction,"getFilename",0,0,String::create(self->getCode().getFilename()))

	//! [ESMF] String UserFunction.getCode()
	ESMF_DECLARE(t,UserFunction,"getCode",0,0,String::create(self->getCode().getCodeString()))

	//! [ESMF] Number|false UserFunction.getMaxParamCount()
	ES_MFUNCTION_DECLARE(t,UserFunction,"getMaxParamCount",0,0,{
		if(self->getMaxParamCount()<0 )
			return Bool::create(false);
		return Number::create(self->getMaxParamCount());
	})

	//! [ESMF] Number UserFunction.getMinParamCount()
	ESMF_DECLARE(t,UserFunction,"getMinParamCount",0,0, Number::create(self->getMinParamCount()))

	//! [ESMF] String UserFunction._asm()
	ESMF_DECLARE(t,UserFunction,"_asm",0,0, String::create(self->getInstructions().toString()))

}

//! (ctor)
UserFunction::UserFunction(const UserFunction & other) : 
		ExtObject(other),codeFragment(other.codeFragment),
		paramCount(other.paramCount),minParamValueCount(other.minParamValueCount),maxParamValueCount(other.maxParamValueCount),
		instructions(other.instructions){
}

//! (ctor)
UserFunction::UserFunction() : 
		ExtObject(getTypeObject()),paramCount(0),minParamValueCount(0),maxParamValueCount(0) {
//	initInstructions();
	//ctor
}
//! (ctor)
UserFunction::~UserFunction(){}
