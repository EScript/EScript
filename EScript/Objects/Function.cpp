// Function.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Function.h"
#include <iostream>
#include <sstream>
#include "../EScript.h"

using namespace EScript;

//! (ctor)
Function::Function(identifierId _originalName, int _minParamCountint, int _maxParamCount,functionPtr _fnptr) : 
		Object(),fnptr(_fnptr),minParamCount(_minParamCountint),maxParamCount(_maxParamCount),originalName(_originalName) {
}
		
//! (ctor)
Function::Function(functionPtr _fnptr) : 
		Object(),fnptr(_fnptr),minParamCount(0),maxParamCount(-1),originalName(0) {
}

//! (ctor)
Function::~Function() {
}
