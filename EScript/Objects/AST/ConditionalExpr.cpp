// ConditionalExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ConditionalExpr.h"
#include "../../Parser/CompilerContext.h"
#include <sstream>

using namespace EScript;

//! (ctor)
ConditionalExpr::ConditionalExpr(ObjPtr _condition,ObjPtr _action,ObjPtr _elseAction):
		condition(_condition),action(_action),elseAction(_elseAction) {
	//ctor
}

//! (dtor)
ConditionalExpr::~ConditionalExpr() {
	//dtor
}

//! ---|> [Object]
std::string ConditionalExpr::toString()const {
	std::ostringstream sprinter;
	sprinter << "if("<<condition.toString()<< ")"<<action.toString();
	if (!elseAction.isNull())
		sprinter << " else "<<elseAction.toString();
	sprinter << " ";
	return sprinter.str();
}

//! ---|> Statement
void ConditionalExpr::_asm(CompilerContext & ctxt){
	if(condition.isNull()){
		if(elseAction.isNotNull()){
			elseAction->_asm(ctxt);
		}
	}else{
		const uint32_t elseMarker = ctxt.createMarker();
	
		condition->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
		if(action.isNotNull()){
			action->_asm(ctxt);
		}
		
		if(elseAction.isNotNull()){
			const uint32_t endMarker = ctxt.createMarker();
			ctxt.addInstruction(Instruction::createJmp(endMarker));
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			elseAction->_asm(ctxt);
			ctxt.addInstruction(Instruction::createSetMarker(endMarker));
		}else{
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
		}
	}
}