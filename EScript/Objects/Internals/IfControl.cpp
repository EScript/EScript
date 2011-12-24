// IfControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "IfControl.h"

using namespace EScript;

//! (ctor)
IfControl::IfControl(Object * condition,const Statement & action,const Statement & elseAction):
		conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
	//ctor
}

//! (dtor)
IfControl::~IfControl() {
	//dtor
}
