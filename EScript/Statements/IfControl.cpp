// IfControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfControl.h"
#include <iostream>
#include <sstream>
#include "../Runtime/Runtime.h"
using namespace EScript;

//! (ctor)
IfControl::IfControl(Object * condition,Object * action,Object * elseAction):
		conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
	//ctor
}

//! (dtor)
IfControl::~IfControl() {
	//dtor
}

//! ---|> [Object]
std::string IfControl::toString()const {
	std::ostringstream sprinter;
	sprinter << "if("<<conditionRef.toString()<< ")"<<actionRef.toString();
	if (!elseActionRef.isNull())
		sprinter << " else "<<elseActionRef.toString();
	sprinter << " ";
	return sprinter.str();
}

//! ---|> [Object]
Object * IfControl::execute(Runtime & rt) {
	if (!conditionRef.isNull()) {
		ObjRef conResult=rt.executeObj(conditionRef.get());
		if(!rt.assertNormalState(this)) return NULL;

		if (conResult.toBool()) {
			return actionRef.isNull()?NULL:rt.executeObj(actionRef.get());
		}
	}
	if (!elseActionRef.isNull()) {
		return rt.executeObj(elseActionRef.get());
	}
	return NULL;
}
