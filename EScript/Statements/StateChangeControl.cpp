// StateChangeControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StateChangeControl.h"
#include "../Runtime/Runtime.h"
#include <sstream>

using namespace EScript;

//! (static) Factory
StateChangeControl * StateChangeControl::createBreakControl(){
	return new StateChangeControl(Runtime::STATE_BREAKING,NULL);
}

//! (static) Factory
StateChangeControl * StateChangeControl::createContinueControl(){
	return new StateChangeControl(Runtime::STATE_CONTINUE,NULL);
}

//! (static) Factory
StateChangeControl * StateChangeControl::createReturnControl(Object * expression){
	return new StateChangeControl(Runtime::STATE_RETURNING,expression);
}

//! (static) Factory
StateChangeControl * StateChangeControl::createThrowControl(Object * expression){
	return new StateChangeControl(Runtime::STATE_EXCEPTION,expression);
}

//! (static) Factory
StateChangeControl * StateChangeControl::createExitControl(Object * expression){
	return new StateChangeControl(Runtime::STATE_EXITING,expression);
}

//! (ctor)
StateChangeControl::StateChangeControl(Runtime::state_t _type,Object * exp):
		resultExpRef(exp),type(_type) {
	//ctor
}

//! (dtor)
StateChangeControl::~StateChangeControl() {
	//dtor
}

//! ---|> [Object]
std::string StateChangeControl::toString()const {
	std::ostringstream sprinter;
	switch(type){
		case Runtime::STATE_BREAKING:{
			sprinter<<"break";
			break;
		}
		case Runtime::STATE_CONTINUE:{
			sprinter<<"continue";
			break;
		}
		case Runtime::STATE_RETURNING:{
			sprinter<<"retun";
			break;
		}
		case Runtime::STATE_EXCEPTION:{
			sprinter<<"throw";
			break;
		}
		case Runtime::STATE_EXITING:{
			sprinter<<"exit";
			break;
		}
		default:{
		}
	}
	if( !resultExpRef.isNull())
		sprinter<<" "<<resultExpRef.toString();
	return sprinter.str();
}

//! ---|> [Object]
Object * StateChangeControl::execute(Runtime & rt) {
	switch(type){
		case Runtime::STATE_BREAKING:{
			rt.setState(Runtime::STATE_BREAKING);
			return NULL;
		}
		case Runtime::STATE_CONTINUE:{
			rt.setState(Runtime::STATE_CONTINUE);
			return NULL;
		}
		case Runtime::STATE_RETURNING:{
			ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
			if(!rt.assertNormalState(this)) return NULL;
			rt.setReturnState(valueRef);
			return NULL;
		}
		case Runtime::STATE_EXCEPTION:{
			ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
			if(!rt.assertNormalState(this)) return NULL;
			rt.setExceptionState(valueRef);
			return NULL;
		}
		case Runtime::STATE_EXITING:{
			ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
			if(!rt.assertNormalState(this)) return NULL;
			rt.setExitState(valueRef);
			return NULL;
		}
		default:{
		}
	}
	return NULL;
}
