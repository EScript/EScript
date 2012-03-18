// TryCatchStatement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "TryCatchStatement.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;
using namespace EScript::AST;


//! ---|> Statement
void TryCatchStatement::_asm(CompilerContext & ctxt){
	

	const uint32_t catchMarker = ctxt.createMarker();
	const uint32_t endMarker = ctxt.createMarker();
	
	// try
	// ------
	ctxt.pushSetting_marker(CompilerContext::EXCEPTION_MARKER,catchMarker);
	ctxt.addInstruction(Instruction::createSetExceptionHandler(catchMarker)); 

	// collect all variables that are declared inside the try-block (excluding nested try-blocks)
	std::vector<size_t> collectedVariableIndices;
	ctxt.pushLocalVarsCollector(&collectedVariableIndices);
	tryBlock->_asm(ctxt);
	ctxt.popLocalVarsCollector();

	ctxt.addInstruction(Instruction::createJmp(endMarker));
	ctxt.popSetting(); // EXCEPTION_MARKER
	
	// catch
	// ------
	ctxt.addInstruction(Instruction::createSetMarker(catchMarker));
	// reset catchMarker
	ctxt.addInstruction(Instruction::createSetExceptionHandler(ctxt.getCurrentMarker(CompilerContext::EXCEPTION_MARKER))); 

	// clear all variables defined inside try block
	for(std::vector<size_t>::const_iterator it = collectedVariableIndices.begin(); it!=collectedVariableIndices.end();++it){
		ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
	}
	
	// clear value stack
	// define exception variable
	if(!exceptionVariableName.empty()){
		std::set<StringId> varSet;
		varSet.insert(exceptionVariableName);
		ctxt.pushSetting_localVars(varSet);
	}
	// load exception variable with exception object ( exceptionVariableName = __result )
	ctxt.addInstruction(Instruction::createGetLocalVariable(2));  
	ctxt.addInstruction(Instruction::createAssignLocal(ctxt.getCurrentVarIndex(exceptionVariableName))); 
	
	// execute catch block
	catchBlock->_asm(ctxt);
	// pop exception variable
	if(!exceptionVariableName.empty()){
		ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(exceptionVariableName)));
		ctxt.popSetting(); // variable
	}
	// end:
	ctxt.addInstruction(Instruction::createSetMarker(endMarker));


}