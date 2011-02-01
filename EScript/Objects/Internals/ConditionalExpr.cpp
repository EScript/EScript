// ConditionalExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ConditionalExpr.h"
#include <iostream>
#include <sstream>
#include "../../Runtime/Runtime.h"

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
//
////! ---|> [Object]
//Object * ConditionalExpr::execute(Runtime & rt) {
//	if (!condition.isNull()) {
//		ObjRef conResult=rt.executeObj(condition.get());
//		if(!rt.assertNormalState(this)) return NULL;
//
//		if (conResult.toBool()) {
//			return action.isNull()?NULL:rt.executeObj(action.get());
//		}
//	}
//	if (!elseAction.isNull()) {
//		return rt.executeObj(elseAction.get());
//	}
//	return NULL;
//}
