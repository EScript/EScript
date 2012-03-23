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
UserFunctionExpr::Parameter::Parameter(const StringId & _name,Object * defaultValueExpression/*=NULL*/,Object * type/*=NULL*/):
	name(_name),defaultValueExpressionRef(defaultValueExpression),typeRef(type),multiParam(false){
}

std::string UserFunctionExpr::Parameter::toString()const{
	std::string s=typeRef.toString()+' '+name.toString();
	if(!defaultValueExpressionRef.isNull())
		s+='='+defaultValueExpressionRef->toDbgString();
	if(multiParam)
		s+='*';
	return s;
}
UserFunctionExpr::Parameter * UserFunctionExpr::Parameter::clone()const{
	Parameter * p = new Parameter( name,
								defaultValueExpressionRef.isNotNull() ? defaultValueExpressionRef->getRefOrCopy() : NULL,
								typeRef.isNotNull() ? typeRef->getRefOrCopy() : NULL);
	p->setMultiParam(isMultiParam());
	return p;
}
// ------------------------------------------------------------


//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::BlockStatement * block):
		ExtObject(), posInFile(0),codeLen(0) {

	setBlock(block);
	//ctor
}
//! (ctor)
UserFunctionExpr::UserFunctionExpr(AST::BlockStatement * block,const std::vector<ObjRef> & _sConstrExpressions):
		ExtObject(), sConstrExpressions(_sConstrExpressions.begin(),_sConstrExpressions.end()),posInFile(0),codeLen(0) {

	setBlock(block);
	//ctor
}


void UserFunctionExpr::setBlock(AST::BlockStatement * _block){
	blockRef=_block;
}

//! ---|> [Object]
std::string UserFunctionExpr::toDbgString()const {
	std::ostringstream sprinter;
	sprinter << "fn(";
	int nr=0;
	for (parameterList_t::const_iterator it=params.begin();it!=params.end();++it) {
		if (nr++>0) sprinter<< ',';
		if ( !(*it).getName().empty() ) sprinter<<(*it).toString();
	}
	sprinter << "){...} "<<'['+getFilename()<<':'<<getLine()<<']';
	return sprinter.str();
}


std::string UserFunctionExpr::getFilename()const {
	return blockRef.isNull() ? "" : blockRef->getFilename();
}


int UserFunctionExpr::getLine()const	{
	return blockRef.isNull() ? -1 : blockRef->getLine();
}

void UserFunctionExpr::setCodeString(const EPtr<String> & _fileString,size_t _begin,size_t _codeLen){
	fileString = _fileString.get();
	posInFile = _begin;
	codeLen = _codeLen;
}

std::string UserFunctionExpr::getCode()const{
	return fileString->toString().substr(posInFile,codeLen);
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

