// SetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "SetAttribute.h"

using namespace EScript;

//! (ctor)
SetAttribute::SetAttribute(Object * obj,identifierId _attrId,Object * _valueExp,assignType_t _assignType,int _line):
		objExpr(obj),valueExpr(_valueExp),attrId(_attrId),assignType(_assignType),line(_line) {
	//ctor
}

//! (dtor)
SetAttribute::~SetAttribute() {
	//dtor
}

//! ---|> [Object]
std::string SetAttribute::toString()const {
	std::string s="";
	if (!objExpr.isNull()) {
		s+=objExpr.toString();
	} else s+="_";
	s+="."+getAttrName();
	switch(assignType){
		case ASSIGN:
			s+="=";
			break;
		case SET_OBJ_ATTRIBUTE:
			s+=":=";
			break;
		case SET_TYPE_ATTRIBUTE:
			s+="::=";
			break;
		default:
			s+="?=";
	}
	s+="("+valueExpr.toString()+") ";
	return s;
}
			
