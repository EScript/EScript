// BlockStatement.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "BlockStatement.h"
#include "../../Parser/CompilerContext.h"

#include <sstream>
#include <utility>

using namespace EScript;
using namespace EScript::AST;

//! (ctor)
BlockStatement::BlockStatement(int lineNr):vars(NULL),line(lineNr){
	//ctor
}

//! (dtor)
BlockStatement::~BlockStatement() {
	delete vars;
	//dtor
}

bool BlockStatement::declareVar(StringId id) {
	if(vars==NULL){
		vars=new declaredVariableMap_t();
	}
	std::pair<declaredVariableMap_t::iterator,bool> result= vars->insert(id);
	return result.second;
}

void BlockStatement::addStatement(const Statement & s) {
	if(s.isValid())
		statements.push_back(s);
}

