// UserFunction.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "UserFunction.h"
#include "Internals/Block.h"
#include <sstream>

using namespace EScript;


UserFunction::Parameter::Parameter(identifierId _name,Object * defaultValueExpression/*=NULL*/,Object * type/*=NULL*/):
	name(_name),defaultValueExpressionRef(defaultValueExpression),typeRef(type),multiParam(false){
}
UserFunction::Parameter::~Parameter(){
}


std::string UserFunction::Parameter::toString()const{
	std::string s=typeRef.toString()+" "+EScript::identifierIdToString(name);
	if(!defaultValueExpressionRef.isNull())
		s+="="+defaultValueExpressionRef->toDbgString();
	if(multiParam)
		s+="*";
	return s;
}
// ------------------------------------------------------------

//! (ctor)
UserFunction::UserFunction(parameterList_t * _params,Block * block):
		ExtObject(), params(_params) {

	setBlock(block);
	//ctor
}
//! (ctor)
UserFunction::UserFunction(parameterList_t * _params,Block * block,const std::vector<ObjRef> & _sConstrExpressions):
		ExtObject(), params(_params),sConstrExpressions(_sConstrExpressions.begin(),_sConstrExpressions.end()) {

	setBlock(block);
	//ctor
}

//! (dtor)
UserFunction::~UserFunction() {
	for (parameterList_t::iterator it=params->begin();it!=params->end();++it) {
		delete (*it);
	}
	delete params;
	//dtor
}

void UserFunction::setBlock(Block * _block){
	blockRef=_block;
}

//! ---|> [Object]
std::string UserFunction::toString()const {
	std::ostringstream sprinter;
	sprinter << "fn(";
	int nr=0;
	for (parameterList_t::const_iterator it=params->begin();it!=params->end();++it) {
		if (nr++>0) sprinter<< ",";
		if ( (*it) ) sprinter<<(*it)->toString();
	}
	sprinter << ")"<<blockRef.toString() << " ";
	return sprinter.str();
}

//! ---|> [Object]
std::string UserFunction::toDbgString()const {
	std::ostringstream sprinter;
	sprinter << "fn(";
	int nr=0;
	for (parameterList_t::const_iterator it=params->begin();it!=params->end();++it) {
		if (nr++>0) sprinter<< ",";
		if ( (*it) ) sprinter<<(*it)->toString();
	}
	sprinter << "){...} "<<"["+getFilename()<<":"<<getLine()<<"]";
	return sprinter.str();
}


std::string UserFunction::getFilename()const {
	return blockRef.isNull() ? "" : blockRef->getFilename();
}


int UserFunction::getLine()const	{
	return blockRef.isNull() ? -1 : blockRef->getLine();
}
