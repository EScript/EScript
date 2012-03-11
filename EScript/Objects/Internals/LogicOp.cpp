// LogicOp.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LogicOp.h"
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
void LogicOp::_asmOut(std::ostream & out){
	static int markerNr = 0;
	
	switch(op){
	case NOT:{
		leftRef->_asmOut(out);
		out << "not\n";
		break;
	}
	case OR:{
		const int marker = ++markerNr;
		const int endMarker = ++markerNr;

		leftRef->_asmOut(out);
		out << "jmpOnTrue marker"<<marker<<":\n";
		rightRef->_asmOut(out);
		out << "jmpOnTrue marker"<<marker<<":\n";
		out << "push (Bool) false\n";
		out << "jmp marker"<<endMarker<<":\n";
		out << "marker" << marker<<":\n";
		out << "push (Bool) true\n";
		out << "marker" << endMarker<<":\n";
		break;
	}
	default:
	case AND:{
		const int marker = ++markerNr;
		const int endMarker = ++markerNr;

		leftRef->_asmOut(out);
		out << "jmpOnFalse marker"<<marker<<":\n";
		rightRef->_asmOut(out);
		out << "jmpOnFalse marker"<<marker<<":\n";
		out << "push (Bool) true\n";
		out << "jmp marker"<<endMarker<<":\n";
		out << "marker" << marker<<":\n";
		out << "push (Bool) false\n";
		out << "marker" << endMarker<<":\n";
		break;
	}
	}
	
}