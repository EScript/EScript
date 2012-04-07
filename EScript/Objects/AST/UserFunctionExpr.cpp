// UserFunctionExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunctionExpr.h"
#include "../AST/BlockStatement.h"
#include "../../EScript.h"
#include <sstream>

using namespace EScript;

//! (ctor) UserFunctionExpr::Parameter
UserFunctionExpr::Parameter::Parameter(const StringId & _name,Object * defaultValueExpression,std::vector<ObjRef> & _typeExpressions):
		name(_name),defaultValueExpressionRef(defaultValueExpression),multiParam(false){
			
	std::swap(_typeExpressions,typeExpressions);
}


// ------------------------------------------------------------


//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::BlockStatement * block):
		ExtObject(){

	setBlock(block);
	//ctor
}
//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::BlockStatement * block,const std::vector<ObjRef> & _sConstrExpressions):
		ExtObject(), sConstrExpressions(_sConstrExpressions.begin(),_sConstrExpressions.end()) {

	setBlock(block);
	//ctor
}


void UserFunctionExpr::setBlock(AST::BlockStatement * _block){
	blockRef=_block;
}


int UserFunctionExpr::getLine()const	{
	return blockRef.isNull() ? -1 : blockRef->getLine();
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

