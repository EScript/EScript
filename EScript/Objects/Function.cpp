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
Function::Function(functionPtr _fnptr):ExtObject(),fnptr(_fnptr) {
	//ctor
}

//! (ctor)
Function::~Function() {
	//dtor
}
