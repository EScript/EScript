// GetAttributeExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "GetAttributeExpr.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;
using namespace EScript::AST;

//! (ctor)
GetAttributeExpr::GetAttributeExpr(Object * obj,StringId _attrId):
		objRef(obj),attrId(_attrId) {
	//ctor
}

//! (ctor)
GetAttributeExpr::GetAttributeExpr(Object * obj,const std::string & attrName):
		objRef(obj),attrId(StringId(attrName)) {
	//ctor
}

//! (dtor)
GetAttributeExpr::~GetAttributeExpr() {
	//dtor
}

/*!	---|> [Object]	*/
std::string GetAttributeExpr::toString()const {
	std::string s;
	if (!objRef.isNull()) {
		s+=objRef.toString()+'.';
	}
	s += attrId.toString();
	return s;
}

//! ---|> Statement
void GetAttributeExpr::_asm(CompilerContext & ctxt){
	if(objRef.isNotNull()){
		objRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createGetAttribute(attrId));
	}else{
		const int localVarIndex = ctxt.getCurrentVarIndex(attrId);
		if(localVarIndex>=0){
			ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
		}else{
			ctxt.addInstruction(Instruction::createGetVariable(attrId));
		}
	}

}