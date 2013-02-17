// UserFunctionExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunctionExpr.h"
#include "../../Basics.h"
#include "Block.h"
#include <sstream>

namespace EScript{
namespace AST{

//! (ctor) UserFunctionExpr::Parameter
UserFunctionExpr::Parameter::Parameter(const StringId & _name,ptr_t defaultValueExpression,refArray_t && _typeExpressions):
		name(_name),defaultValueExpressionRef(defaultValueExpression),typeExpressions(_typeExpressions),multiParam(false){
}

// ------------------------------------------------------------

//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::Block * block,const refArray_t & _sConstrExpressions,int _line):
		ASTNode(TYPE_USER_FUNCTION_EXPRESSION,true,_line), 
		blockRef(block), sConstrExpressions(_sConstrExpressions){
	//ctor
}

}
}
