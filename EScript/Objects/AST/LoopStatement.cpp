// LoopStatement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LoopStatement.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;
using namespace EScript::AST;

//! (ctor)
LoopStatement::LoopStatement( 	const Statement & _initStmt,
									Object * _preConditionExpression,
									const Statement & _action,
									Object * _postConditionExpression,
									const Statement & _increaseStmt) : 
		initStmt(_initStmt),preConditionExpression(_preConditionExpression),action(_action),postConditionExpression(_postConditionExpression),
		increaseStmt(_increaseStmt){
	//ctor
}

//! ---|> Statement
void LoopStatement::_asm(CompilerContext & ctxt){
	const uint32_t loopBegin = ctxt.createMarker();
	const uint32_t loopEndMarker = ctxt.createMarker();
	
	if(initStmt.isValid()){
		ctxt.setLine(initStmt.getLine());
		initStmt._asm(ctxt);
	}
	ctxt.addInstruction(Instruction::createSetMarker(loopBegin));
	
	if(preConditionExpression.isNotNull()){
		preConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(loopEndMarker));
	}
	action._asm(ctxt);
	
	if(postConditionExpression.isNotNull()){ // increaseStmt is ignored!
		postConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnTrue(loopBegin));
	}else{
		if(increaseStmt.isValid()){
			increaseStmt._asm(ctxt);
		}
		ctxt.addInstruction(Instruction::createJmp(loopBegin));
	}
	
	ctxt.addInstruction(Instruction::createSetMarker(loopEndMarker));

}