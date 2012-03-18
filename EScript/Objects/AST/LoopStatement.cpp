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
	const uint32_t loopContinueMarker = ctxt.createMarker();
	
	if(initStmt.isValid()){
		ctxt.setLine(initStmt.getLine());
		initStmt._asm(ctxt);
	}
	ctxt.addInstruction(Instruction::createSetMarker(loopBegin));
	
	if(preConditionExpression.isNotNull()){
		preConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(loopEndMarker));
	}
	ctxt.pushSetting_marker( CompilerContext::BREAK_MARKER ,loopEndMarker);
	ctxt.pushSetting_marker( CompilerContext::CONTINUE_MARKER ,loopContinueMarker);
	action._asm(ctxt);
	ctxt.popSetting();
	ctxt.popSetting();
	
	if(postConditionExpression.isNotNull()){ // increaseStmt is ignored!
		ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
		postConditionExpression->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnTrue(loopBegin));
	}else{
		ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
		if(increaseStmt.isValid()){
			increaseStmt._asm(ctxt);
		}
		ctxt.addInstruction(Instruction::createJmp(loopBegin));
	}
	
	ctxt.addInstruction(Instruction::createSetMarker(loopEndMarker));

}