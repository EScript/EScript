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
BlockStatement::BlockStatement(int lineNr):
		filenameId(0),vars(NULL),line(lineNr),
		continuePos(POS_DONT_HANDLE),breakPos(POS_DONT_HANDLE),exceptionPos(POS_DONT_HANDLE),jumpPosA(0) {
	//ctor
}

//! (dtor)
BlockStatement::~BlockStatement() {

	delete vars;
	//dtor
}

//! ---|> [Object]
std::string BlockStatement::toString()const {
	static int depth=0;
	std::ostringstream sprinter;
	sprinter << "{" <<  std::endl;
	depth++;
	for ( cStatementCursor c = statements.begin();  c != statements.end(); ++c) {
		for (int i=0;i<depth;++i) sprinter << "\t";
			sprinter <<".";
//		sprinter <<  c->getObject()->toString() <<  std::endl;
	}
	depth--;
	for (int i=0;i<depth;++i) sprinter << "\t";
	sprinter << "}";
	return sprinter.str();
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

//! ---|> Object
void BlockStatement::_asm(CompilerContext & ctxt){

	if(vars) 
		ctxt.pushSetting_localVars(*vars);

	for ( statementCursor c = statements.begin();  c != statements.end(); ++c) {
		c->_asm(ctxt);
	}
	if(vars){
		for(std::set<StringId>::const_iterator it = vars->begin();it!=vars->end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(*it)));
		}
		ctxt.popSetting();
	}

}
