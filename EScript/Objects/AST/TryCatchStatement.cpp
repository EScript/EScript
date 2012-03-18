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
	

//////	const uint32_t exceptionMarker = ctxt.createMarker();
//////	const uint32_t endMarker = ctxt.createMarker();
//////	
//////	ctxt.addInstruction()
//////	tryBlock->_asm(ctxt);
//		
//		ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
//		if(actionRef.isValid()){
//			actionRef._asm(ctxt);
//		}
//		
//		if(elseActionRef.isValid()){
//			const uint32_t endMarker = ctxt.createMarker();
//			ctxt.addInstruction(Instruction::createJmp(endMarker));
//			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
//			elseActionRef._asm(ctxt);
//			ctxt.addInstruction(Instruction::createSetMarker(endMarker));
//		}else{
//			ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
//		}
//	}
}