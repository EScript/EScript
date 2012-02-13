// SetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "SetAttribute.h"

using namespace EScript;

//! static
SetAttribute * SetAttribute::createAssignment(Object * obj,identifierId attrId,Object * valueExp,int _line){
	SetAttribute * sa  = new SetAttribute(obj,attrId,valueExp,0,_line) ;
	sa->assign = true;
	return sa;
}

//! (ctor)
SetAttribute::SetAttribute(Object * obj,identifierId _attrId,Object * _valueExp,Attribute::flag_t _attrFlags,int _line):
		objExpr(obj),valueExpr(_valueExp),attrId(_attrId),attrFlags(_attrFlags),line(_line),assign(false) {
	//ctor
}

//! (dtor)
SetAttribute::~SetAttribute() {
	//dtor
}

//! ---|> [Object]
std::string SetAttribute::toString()const {
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
