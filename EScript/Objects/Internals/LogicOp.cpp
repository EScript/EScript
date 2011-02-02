// LogicOp.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "LogicOp.h"
#include "../Bool.h"
#include "../../Runtime/Runtime.h"
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
