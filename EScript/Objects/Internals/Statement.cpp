// Statement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Statement.h"
#include "../../Parser/CompilerContext.h"

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


//! ---o EXPERIMENTAL !!!!!
void Statement::_asm(CompilerContext & ctxt){
	if(expression.isNotNull()){
		expression->_asm(ctxt);
		if(type == TYPE_EXPRESSION)
			ctxt.out<<"pop\n";
//		out<<"\n";
	}

}


}
