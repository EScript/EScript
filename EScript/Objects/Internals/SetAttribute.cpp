// SetAttribute.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "SetAttribute.h"
#include "../../Runtime/Runtime.h"
#include "../Void.h"
#include <iostream>

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
	}
	s+="("+valueExpr.toString()+") ";
	return s;
}
//
////! ---|> [Object]
//Object * SetAttribute::execute(Runtime & rt) {
//	if(line>0)
//		rt.setCurrentLine(line);
//	ObjRef valueRef=NULL;
//	if (!valueExpr.isNull()) {
//		valueRef=rt.executeObj(valueExpr.get());
//		if(!rt.assertNormalState(this))
//			return NULL;
//
//		/// Bug[20070703] fixed:
//		if(!valueRef.isNull()){
//			valueRef=valueRef->getRefOrCopy();
//		}else{
//			valueRef=Void::get();
//		}
//	}
//	/// Local variable
//	if (objExpr.isNull()) {
//		rt.assignToVariable(attrId,valueRef.get());
//		return valueRef.detachAndDecrease();
//	}
//	/// obj.ident
//
//	ObjRef obj2=rt.executeObj(objExpr.get());
//	if(!rt.assertNormalState(this))
//		return NULL;
//
//	if(obj2.isNull())
//		obj2=Void::get();
//	if(assignType == ASSIGN){
//		if(!obj2->assignAttribute(attrId,valueRef.get())){
//			rt.warn(std::string("Unkown attribute \"")+identifierIdToString(attrId)+"\" ("+
//					(objExpr.isNull()?"":objExpr->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
//			if(!obj2->setObjAttribute(attrId,valueRef.get())){
//				rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
//						(objExpr.isNull()?"":objExpr->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
//			}
//		}
//	}else if(assignType == SET_OBJ_ATTRIBUTE){
//		if(!obj2->setObjAttribute(attrId,valueRef.get()))
//			rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
//					(objExpr.isNull()?"":objExpr->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
//	}else if(assignType == SET_TYPE_ATTRIBUTE){
//		Type * t=obj2.toType<Type>();
//		if(t){
//			t->setTypeAttribute(attrId,valueRef.get());
//		}else{
//			rt.warn(std::string("Can not set typeAttr to non-Type-Object: \"")+identifierIdToString(attrId)+"\" ("+
//					(objExpr.isNull()?"":objExpr->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")"
//					+"Setting objAttr instead.");
//			if(!obj2->setObjAttribute(attrId,valueRef.get())){
//				rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
//						(objExpr.isNull()?"":objExpr->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
//			}
//		}
//	}
//	return valueRef.detachAndDecrease();
//}
