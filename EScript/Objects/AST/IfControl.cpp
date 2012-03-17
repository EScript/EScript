// IfControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfControl.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;

//! (ctor)
IfControl::IfControl(Object * condition,const Statement & action,const Statement & elseAction):
		conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
	//ctor
}

//! (dtor)
IfControl::~IfControl() {
	//dtor
}

//! ---|> Statement
void IfControl::_asm(CompilerContext & ctxt){
	
//	ctxt.out << "//<IfControl '"<<toString()<<"'\n";
	if(conditionRef.isNull()){
		if(elseActionRef.isValid()){
			elseActionRef._asm(ctxt);
		}
	}else{
		const uint32_t elseMarker = ctxt.createMarker();
		
		
		conditionRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
//		ctxt.out << "jmpIfFalse "<<elseMarker<<":\n";
		if(actionRef.isValid()){
			actionRef._asm(ctxt);
		}
		
		if(elseActionRef.isValid()){
			const uint32_t endMarker = ctxt.createMarker();
			ctxt.addInstruction(Instruction::createJmp(endMarker));
//			ctxt.out << "jmp "<<endMarker<<"\n";
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
//			ctxt.out << elseMarker<<":\n";
			elseActionRef._asm(ctxt);
			ctxt.addInstruction(Instruction::createSetMarker(endMarker));
//			ctxt.out << endMarker<<":\n";
		}else{
			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
//			ctxt.out << elseMarker<<":\n";
		}
		
		
	
	}
//	ctxt.out << "//IfControl>\n";

}