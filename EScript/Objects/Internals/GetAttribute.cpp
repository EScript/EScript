// GetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "GetAttribute.h"
#include "../../Runtime/Runtime.h"
#include "../Void.h"

#include <iostream>

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
//
////! ---|> [Object]
//Object * GetAttribute::execute(Runtime & rt) {
//	ObjRef resultRef=NULL;
//	//std::cout << " <get:"<<this->toString()<<":"<<attrId<<"> ";
//	/// Local variable
//	if (objRef.isNull()) {
//		rt.setCallingObject(NULL);
////        if (global)
////            resultRef=rt.getGlobalVariable(attrId);
////        else
//		resultRef=rt.getVariable(attrId);
//		if (resultRef.isNull())
//			rt.warn("Unknown Variable:"+toString());
//
//		// obj.ident
//	} else {
//		ObjRef obj2Ref=rt.executeObj(objRef.get());
//		if(!rt.assertNormalState(this))
//			return NULL;
//
//		if(obj2Ref.isNull())
//			obj2Ref=Void::get();
//
//		rt.setCallingObject(obj2Ref.get());
//		resultRef=obj2Ref->getAttribute(attrId);
//		if (resultRef.isNull()) {
//			rt.warn("Member not set:"+toString());
//		}
//	}
//	return resultRef.detachAndDecrease();
//}
