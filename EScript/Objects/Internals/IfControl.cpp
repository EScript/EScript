// IfControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfControl.h"

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
	
	ctxt.out << "//<IfControl '"<<toString()<<"'\n";
	if(conditionRef.isNull()){
		if(elseActionRef.isValid()){
			elseActionRef._asm(ctxt);
		}
	}else{
		const CompilerContext::markerId_t elseMarker = ctxt.createMarkerId("ifElse");
		
		
		conditionRef->_asm(ctxt);
		ctxt.out << "jmpIfFalse "<<elseMarker<<":\n";
		if(actionRef.isValid()){
			actionRef._asm(ctxt);
		}
		
		if(elseActionRef.isValid()){
			const CompilerContext::markerId_t endMarker = ctxt.createMarkerId("ifEnd");
			ctxt.out << "jmp "<<endMarker<<"\n";
			ctxt.out << elseMarker<<":\n";
			elseActionRef._asm(ctxt);
			ctxt.out << endMarker<<":\n";
		}else{
			ctxt.out << elseMarker<<":\n";
		}
		
		
	
	}
	ctxt.out << "//IfControl>\n";

}