// LoopExpression.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LoopExpression.h"
#include "../../Parser/CompilerContext.h"

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
	const CompilerContext::marker_t loopBegin = ctxt.createMarker("loopBegin");
	const CompilerContext::marker_t loopEndMarker = ctxt.createMarker("loopEnd");
	
//	ctxt.out << "//<LoopExpression '"<<toString()<<"'\n";
	if(initStmt.isValid()){
		ctxt.setLine(initStmt.getLine());
		initStmt._asm(ctxt);
	}

//	ctxt.out << loopBegin<<":\n";
	ctxt.addInstruction(Instruction::createSetMarker(loopBegin));
	
	if(preConditionExpression.isNotNull()){
		preConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(loopEndMarker));
//		ctxt.out << "jmpOnFalse " << loopEndMarker << ":\n";
	}
	action._asm(ctxt);
	
	if(postConditionExpression.isNotNull()){ // increaseStmt is ignored!
		postConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnTrue(loopBegin));
//		ctxt.out << "jmpOnTrue " << loopBegin << ":\n";
	}else{
		if(increaseStmt.isValid()){
			increaseStmt._asm(ctxt);
		}
//		ctxt.out << "jmp " << loopBegin << ":\n";
		ctxt.addInstruction(Instruction::createJmp(loopBegin));
	}
	
	ctxt.addInstruction(Instruction::createSetMarker(loopEndMarker));
//	ctxt.out << loopEndMarker << ":\n";

//	ctxt.out << "//LoopExpression>\n";

}