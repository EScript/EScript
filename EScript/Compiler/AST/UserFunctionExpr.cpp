// UserFunctionExpr.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2012-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
