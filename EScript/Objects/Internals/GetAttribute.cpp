// GetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "GetAttribute.h"

using namespace EScript;

//! (ctor)
GetAttribute::GetAttribute(Object * obj,identifierId _attrId):
		objRef(obj),attrId(_attrId) {
	//ctor
}

//! (ctor)
GetAttribute::GetAttribute(Object * obj,const std::string & attrName):
		objRef(obj),attrId(stringToIdentifierId(attrName)) {
	//ctor
}

//! (dtor)
GetAttribute::~GetAttribute() {
	//dtor
}

/*!	---|> [Object]	*/
std::string GetAttribute::toString()const {
	std::string s="";
	if (!objRef.isNull()) {
		s+=objRef.toString()+".";
	}
	s+=identifierIdToString(attrId);
	return s;
}
