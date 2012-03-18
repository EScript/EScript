// LogicOpExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LogicOpExpr.h"
#include "../../Parser/CompilerContext.h"

#include <sstream>

using namespace EScript;
using namespace EScript::AST;

//! (ctor)
LogicOpExpr::LogicOpExpr(Object * _left,Object * _right,opType_T _op):
		leftRef(_left),rightRef(_right),op(_op) {
	//ctor
}

//! (dtor)
LogicOpExpr::~LogicOpExpr() {
	//dtor
}

//! ---|> [Object]
std::string LogicOpExpr::toString()const {
	std::ostringstream sprinter;
	if (op==NOT)
		sprinter<<"!";
	if (!leftRef.isNull())
		sprinter<<leftRef.toString();
	if (op==OR)
		sprinter<<"||";
	else if (op==AND)
		sprinter<<"&&";
	if (!rightRef.isNull())
		sprinter<<rightRef.toString();
	return sprinter.str();
}

//! ---|> Object
void LogicOpExpr::_asm(CompilerContext & ctxt){
	switch(op){
	case NOT:{
		leftRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createNot());
//		ctxt.out << "not\n";
		break;
	}
	case OR:{
		const uint32_t marker = ctxt.createMarker();
		const uint32_t endMarker = ctxt.createMarker();

		leftRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnTrue(marker));
//		ctxt.out << "jmpOnTrue "<<marker<<":\n";
		rightRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnTrue(marker));
//		ctxt.out << "jmpOnTrue "<<marker<<":\n";
		ctxt.addInstruction(Instruction::createPushBool(false));
//		ctxt.out << "push (Bool) false\n";
		ctxt.addInstruction(Instruction::createJmp(endMarker));
//		ctxt.out << "jmp "<<endMarker<<":\n";
		ctxt.addInstruction(Instruction::createSetMarker(marker));
//		ctxt.out << marker<<":\n";
		ctxt.addInstruction(Instruction::createPushBool(true));
//		ctxt.out << "push (Bool) true\n";
		ctxt.addInstruction(Instruction::createSetMarker(endMarker));
//		ctxt.out << endMarker<<":\n";
		break;
	}
	default:
	case AND:{
		const uint32_t marker = ctxt.createMarker();
		const uint32_t endMarker = ctxt.createMarker();

		leftRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(marker));
//		ctxt.out << "jmpOnFalse "<<marker<<":\n";
		rightRef->_asm(ctxt);
		ctxt.addInstruction(Instruction::createJmpOnFalse(marker));
//		ctxt.out << "jmpOnFalse "<<marker<<":\n";
		ctxt.addInstruction(Instruction::createPushBool(true));
//		ctxt.out << "push (Bool) true\n";
		ctxt.addInstruction(Instruction::createJmp(endMarker));
//		ctxt.out << "jmp "<<endMarker<<":\n";
		ctxt.addInstruction(Instruction::createSetMarker(marker));
//		ctxt.out << marker<<":\n";
		ctxt.addInstruction(Instruction::createPushBool(false));
//		ctxt.out << "push (Bool) false\n";
		ctxt.addInstruction(Instruction::createSetMarker(endMarker));
//		ctxt.out << endMarker<<":\n";
		break;
	}
	}
	
}