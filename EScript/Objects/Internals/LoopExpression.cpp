// LoopExpression.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LoopExpression.h"

using namespace EScript;

//! (ctor)
LoopExpression::LoopExpression( 	const Statement & _initStmt,
									Object * _preConditionExpression,
									const Statement & _action,
									Object * _postConditionExpression,
									const Statement & _increaseStmt) : 
		initStmt(_initStmt),preConditionExpression(_preConditionExpression),action(_action),postConditionExpression(_postConditionExpression),
		increaseStmt(_increaseStmt){
	//ctor
}

//! ---|> Statement
void LoopExpression::_asm(CompilerContext & ctxt){
	const CompilerContext::markerId_t loopBegin = ctxt.createMarkerId("loopBegin");
	const CompilerContext::markerId_t loopEndMarker = ctxt.createMarkerId("loopEnd");
	
	ctxt.out << "//<LoopExpression '"<<toString()<<"'\n";
	if(initStmt.isValid()){
		initStmt._asm(ctxt);
	}
	ctxt.out << loopBegin<<":\n";
	if(preConditionExpression.isNotNull()){
		preConditionExpression->_asm(ctxt);
		ctxt.out << "jmpOnFalse " << loopEndMarker << ":\n";
	}
	action._asm(ctxt);
	
	if(postConditionExpression.isNotNull()){ // increaseStmt is ignored!
		postConditionExpression->_asm(ctxt);
		ctxt.out << "jmpOnTrue " << loopBegin << ":\n";
	}else{
		if(increaseStmt.isValid()){
			increaseStmt._asm(ctxt);
		}
		ctxt.out << "jmp " << loopBegin << ":\n";
	}
	

	ctxt.out << loopEndMarker << ":\n";

	ctxt.out << "//LoopExpression>\n";

}