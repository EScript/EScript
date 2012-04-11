// UserFunctionExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunctionExpr.h"
#include "BlockExpr.h"
#include "../../EScript.h"
#include <sstream>

using namespace EScript;

//! (ctor) UserFunctionExpr::Parameter
UserFunctionExpr::Parameter::Parameter(const StringId & _name,ObjPtr defaultValueExpression,std::vector<ObjRef> & _typeExpressions):
		name(_name),defaultValueExpressionRef(defaultValueExpression),multiParam(false){
			
	std::swap(_typeExpressions,typeExpressions);
}


// ------------------------------------------------------------


//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::BlockExpr * block,const std::vector<ObjRef> & _sConstrExpressions,int _line):
		ExtObject(), blockRef(block), sConstrExpressions(_sConstrExpressions),line(_line) {
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
	int i=0;
	for (parameterList_t::const_iterator it=params.begin();it!=params.end();++it) {
		if( (*it).isMultiParam() || (*it).getDefaultValueExpression() !=NULL )
			break;
		++i;
	}
	return i;
}

