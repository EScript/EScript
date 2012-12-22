// UserFunctionExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunctionExpr.h"
#include "Block.h"
#include "../../EScript.h"
#include <sstream>

using namespace EScript;
using namespace EScript::AST;

//! (ctor) UserFunctionExpr::Parameter
UserFunctionExpr::Parameter::Parameter(const StringId & _name,ptr_t defaultValueExpression,refArray_t & _typeExpressions):
		name(_name),defaultValueExpressionRef(defaultValueExpression),multiParam(false){

	std::swap(_typeExpressions,typeExpressions);
}


// ------------------------------------------------------------


//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::Block * block,const refArray_t & _sConstrExpressions,int _line):
		ASTNode(TYPE_USER_FUNCTION_EXPRESSION,true,_line), 
		blockRef(block), sConstrExpressions(_sConstrExpressions) {
	//ctor
}

int UserFunctionExpr::getMaxParamCount()const{
	if(params.empty()){
		return 0;
	}else if(params.back().isMultiParam()){
		return -1;
	}else
		return params.size();
}

int UserFunctionExpr::getMinParamCount()const{
	int i = 0;
	for(const auto & param : params) {
		if(param.isMultiParam() || param.getDefaultValueExpression() != nullptr) {
			break;
		}
		++i;
	}
	return i;
}
