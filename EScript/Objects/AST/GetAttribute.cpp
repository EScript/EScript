// GetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "GetAttribute.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;

//! (ctor)
GetAttribute::GetAttribute(Object * obj,StringId _attrId):
		objRef(obj),attrId(_attrId) {
	//ctor
}

//! (ctor)
GetAttribute::GetAttribute(Object * obj,const std::string & attrName):
		objRef(obj),attrId(StringId(attrName)) {
	//ctor
}

//! (dtor)
GetAttribute::~GetAttribute() {
	//dtor
}

/*!	---|> [Object]	*/
std::string GetAttribute::toString()const {
	std::string s;
	if (!objRef.isNull()) {
		s+=objRef.toString()+'.';
	}
	s += attrId.toString();
	return s;
}

//! ---|> Statement
void GetAttribute::_asm(CompilerContext & ctxt){
	if(objRef.isNotNull()){
		objRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createGetAttribute(attrId));
	}else{
		const int localVarIndex = ctxt.getVarIndex(attrId);
		if(localVarIndex>=0){
			ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
		}else{
			ctxt.addInstruction(Instruction::createGetVariable(attrId));
		}
	}

}