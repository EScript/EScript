// SetAttributeExpr.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "SetAttributeExpr.h"

using namespace EScript;
using namespace EScript::AST;

//! static
SetAttributeExpr * SetAttributeExpr::createAssignment(Object * obj,StringId attrId,Object * valueExp,int _line){
	SetAttributeExpr * sa  = new SetAttributeExpr(obj,attrId,valueExp,0,_line) ;
	sa->assign = true;
	return sa;
}

//! (ctor)
SetAttributeExpr::SetAttributeExpr(Object * obj,StringId _attrId,Object * _valueExp,Attribute::flag_t _attrFlags,int _line):
		objExpr(obj),valueExpr(_valueExp),attrId(_attrId),attrFlags(_attrFlags),line(_line),assign(false) {
	//ctor
}

//! (dtor)
SetAttributeExpr::~SetAttributeExpr() {
	//dtor
}

//! ---|> [Object]
std::string SetAttributeExpr::toString()const {
	std::string s;
	if (!objExpr.isNull()) {
		s+=objExpr.toString();
	} else s+='_';
	s+='.'+getAttrName();
	if(assign){
		s+='=';
	}else{
		if(attrFlags & (Attribute::CONST_BIT | Attribute::PRIVATE_BIT | Attribute::INIT_BIT)){
			s+="@(";
			if( attrFlags & Attribute::CONST_BIT)		s+="const,";
			if( attrFlags & Attribute::PRIVATE_BIT)		s+="private,";
			if( attrFlags & Attribute::INIT_BIT)		s+="init,";
			s+=')';
		}
		if( attrFlags & Attribute::TYPE_ATTR_BIT)
			s+="::=";
		else
			s+=":=";
	}
	s+='('+valueExpr.toString()+") ";
	return s;
}

