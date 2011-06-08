// Statement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Statement.h"

namespace EScript {

//! (ctor)
Statement::Statement( const Statement & other):
		type(other.type),expression(other.expression),line(other.line) {
	//ctor
}

//! (ctor)
Statement::Statement(type_t _type, ObjPtr _expression):
		type(_type),expression(_expression),line(-1) {
	//ctor
}

//! (ctor)
Statement::Statement(type_t _type):
		type(_type),line(-1) {
	//ctor
}

//! (dtor)
Statement::~Statement() {
	//dtor
}

}
