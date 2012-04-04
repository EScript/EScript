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
	ESMF_DECLARE(t,UserFunction,"getFilename",0,0,String::create(self->getFilename()))

	//! [ESMF] String UserFunction.getCode()
	ESMF_DECLARE(t,UserFunction,"getCode",0,0,String::create(self->getCode()))

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
		ExtObject(other),posInFile(other.posInFile),codeLen(other.codeLen),
		paramCount(other.paramCount),minParamValueCount(other.minParamValueCount),maxParamValueCount(other.maxParamValueCount),
		instructions(other.instructions){
}

//! (ctor)
UserFunction::UserFunction() : 
		ExtObject(getTypeObject()),posInFile(0),codeLen(0),paramCount(0),minParamValueCount(0),maxParamValueCount(0) {
//	initInstructions();
	//ctor
}
//! (ctor)
UserFunction::~UserFunction(){}


//void UserFunction::initInstructions(){
//	for(parameterList_t::const_iterator it = params->begin();it!=params->end();++it){
//		instructions.declareLocalVariable( (**it).getName() );
//	}
//	//! \todo init parameters
//
//}

std::string UserFunction::getFilename()const {
	return "\\todo "; // \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}



void UserFunction::setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen){
	fileString = _fileString.get();
	posInFile = _begin;
	codeLen = _codeLen;
}

std::string UserFunction::getCode()const{
	return fileString.isNotNull() ? fileString->toString().substr(posInFile,codeLen) : std::string("???");
}
