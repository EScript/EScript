// WhileControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "WhileControl.h"
#include "../Runtime/Runtime.h"

#include <sstream>

using namespace EScript;

//! (ctor)
WhileControl::WhileControl(Object * _condition,Object * _action,bool _doWhile):
	condition(_condition),action(_action),doWhile(_doWhile) {
	//ctor
}

//! (dtor)
WhileControl::~WhileControl() {
	//dtor
}

//! ---|> [Object]
std::string WhileControl::toString()const {
	std::ostringstream sprinter;
	if (doWhile)
		sprinter << "do";
	else {
		sprinter << "while(";
		sprinter<<condition.toString();
		sprinter << ")";
	}
	sprinter<<action.toString();
	if (doWhile) {
		sprinter << "while(";
		sprinter<<condition.toString();
		sprinter << ")";
	}

	return sprinter.str();
}

//! ---|> [Object]
Object * WhileControl::execute(Runtime & rt) {
	ObjRef resultRef;

	if (doWhile) {
		while (true) {
			resultRef=NULL;
			resultRef=action.isNull()?NULL:rt.executeObj(action.get());

			if(!rt.checkNormalState()){
				if(rt.getState()==Runtime::STATE_BREAKING){
					rt.resetState();
					break;
				}else if(rt.getState()==Runtime::STATE_CONTINUE){
					rt.resetState();
				}else{
					return NULL;
				}
			}
			if (condition.isNull())
				break;

			ObjRef condResultRef=rt.executeObj(condition.get());
			if(!rt.assertNormalState(this))
					return NULL;

			if(!condResultRef.toBool())
				break;
		}
	} else {
		if (condition.isNull())
			return NULL;
		while (true) {
			ObjRef condRef=rt.executeObj(condition.get());
			if(!rt.assertNormalState(this)) return NULL;

			if (!condRef.toBool())
				break;

			resultRef=NULL;
			resultRef=action.isNull()?NULL:rt.executeObj(action.get());

			if(!rt.checkNormalState()){
				if(rt.getState()==Runtime::STATE_BREAKING){
					rt.resetState();
					break;
				}else if(rt.getState()==Runtime::STATE_CONTINUE){
					rt.resetState();
					continue;
				}else{
					return NULL;
				}
			}
		}
	}
	return resultRef.detachAndDecrease();
}
