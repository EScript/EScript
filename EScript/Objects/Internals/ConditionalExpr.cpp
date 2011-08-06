// ConditionalExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ConditionalExpr.h"

#include <sstream>

using namespace EScript;

//! (ctor)
ConditionalExpr::ConditionalExpr(ObjPtr _condition,ObjPtr _action,ObjPtr _elseAction):
		condition(_condition),action(_action),elseAction(_elseAction) {
	//ctor
}

//! (dtor)
ConditionalExpr::~ConditionalExpr() {
	//dtor
}

//! ---|> [Object]
std::string ConditionalExpr::toString()const {
	std::ostringstream sprinter;
	sprinter << "if("<<condition.toString()<< ")"<<action.toString();
	if (!elseAction.isNull())
		sprinter << " else "<<elseAction.toString();
	sprinter << " ";
	return sprinter.str();
}
