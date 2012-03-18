// Statement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Statement.h"
#include "../../Parser/CompilerContext.h"

namespace EScript {
using namespace EScript::AST;

//! (ctor)
Statement::Statement( const Statement & other):
		type(other.type),expression(other.expression),line(other.line) {
	//ctor
}

//! (ctor)
Statement::Statement(type_t _type, ObjPtr _expression):
		type(_type),expression(_expression),line(-1) {
	//ctor
}

//! (ctor)
Statement::Statement(type_t _type):
		type(_type),line(-1) {
	//ctor
}

//! (dtor)
Statement::~Statement() {
	//dtor
}


//! ---o EXPERIMENTAL !!!!!
void Statement::_asm(CompilerContext & ctxt){
	if(type == TYPE_CONTINUE){
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::CONTINUE_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			std::cout << "\nError: Continue outside a loop!\n"; //! \todo Compiler error
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::CONTINUE_MARKER,variablesToReset);
		for(std::vector<size_t>::const_iterator it = variablesToReset.begin();it!=variablesToReset.end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
		
	}else if(type == TYPE_BREAK){
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::BREAK_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			std::cout << "\nError: Break outside a loop!\n"; //! \todo Compiler error
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::BREAK_MARKER,variablesToReset);
		for(std::vector<size_t>::const_iterator it = variablesToReset.begin();it!=variablesToReset.end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
		
	}else if(expression.isNotNull()){
		ctxt.setLine(line);
		expression->_asm(ctxt);
		if(type == TYPE_EXPRESSION)
			ctxt.addInstruction(Instruction::createPop());
	}

}


}
