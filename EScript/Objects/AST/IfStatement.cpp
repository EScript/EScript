// IfStatement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfStatement.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;
using namespace EScript::AST;

//! (ctor)
IfStatement::IfStatement(Object * condition,const Statement & action,const Statement & elseAction):
		conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
	//ctor
}

//! (dtor)
IfStatement::~IfStatement() {
	//dtor
}

//! ---|> Statement
void IfStatement::_asm(CompilerContext & ctxt){
	if(conditionRef.isNull()){
		if(elseActionRef.isValid()){
			elseActionRef._asm(ctxt);
		}
	}else{
		const uint32_t elseMarker = ctxt.createMarker();
	
		conditionRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
		if(actionRef.isValid()){
			actionRef._asm(ctxt);
		}
		
		if(elseActionRef.isValid()){
			const uint32_t endMarker = ctxt.createMarker();
			ctxt.addInstruction(Instruction::createJmp(endMarker));
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			elseActionRef._asm(ctxt);
			ctxt.addInstruction(Instruction::createSetMarker(endMarker));
		}else{
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
		}
	}
}