// LogicOp.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LogicOp.h"
#include "../../Parser/CompilerContext.h"

#include <sstream>

using namespace EScript;

//! (ctor)
LogicOp::LogicOp(Object * _left,Object * _right,opType_T _op):
		leftRef(_left),rightRef(_right),op(_op) {
	//ctor
}

//! (dtor)
LogicOp::~LogicOp() {
	//dtor
}

//! ---|> [Object]
std::string LogicOp::toString()const {
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
void LogicOp::_asm(CompilerContext & ctxt){
	switch(op){
	case NOT:{
		leftRef->_asm(ctxt);
		ctxt.out << "not\n";
		break;
	}
	case OR:{
		const CompilerContext::markerId_t marker = ctxt.createMarkerId("orMarker");
		const CompilerContext::markerId_t endMarker = ctxt.createMarkerId("orEndMarker");

		leftRef->_asm(ctxt);
		ctxt.out << "jmpOnTrue "<<marker<<":\n";
		rightRef->_asm(ctxt);
		ctxt.out << "jmpOnTrue "<<marker<<":\n";
		ctxt.out << "push (Bool) false\n";
		ctxt.out << "jmp "<<endMarker<<":\n";
		ctxt.out << marker<<":\n";
		ctxt.out << "push (Bool) true\n";
		ctxt.out << endMarker<<":\n";
		break;
	}
	default:
	case AND:{
		const CompilerContext::markerId_t marker = ctxt.createMarkerId("andMarker");
		const CompilerContext::markerId_t endMarker = ctxt.createMarkerId("andEndMarker");

		leftRef->_asm(ctxt);
		ctxt.out << "jmpOnFalse "<<marker<<":\n";
		rightRef->_asm(ctxt);
		ctxt.out << "jmpOnFalse "<<marker<<":\n";
		ctxt.out << "push (Bool) true\n";
		ctxt.out << "jmp "<<endMarker<<":\n";
		ctxt.out << marker<<":\n";
		ctxt.out << "push (Bool) false\n";
		ctxt.out << endMarker<<":\n";
		break;
	}
	}
	
}