// YieldIterator.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "YieldIterator.h"

#include "../EScript.h"

using namespace EScript;
//---

Type* YieldIterator::typeObject=NULL;

//! initMembers
void YieldIterator::init(EScript::Namespace & globals) {
	// YieldIterator ---|> [Object]
	typeObject=new Type(Object::getTypeObject());
	declareConstant(&globals,getClassName(),typeObject);

	//! Object YieldIterator.key()
	ESMF_DECLARE(typeObject,YieldIterator,"key",0,0,self->key())

	//! Object YieldIterator.value()
	ESMF_DECLARE(typeObject,YieldIterator,"value",0,0,self->value())

	//! self YieldIterator.next()
	ESMF_DECLARE(typeObject,YieldIterator,"next",0,0,(self->next(runtime),self))

	//! bool YieldIterator.end()
	ESMF_DECLARE(typeObject,YieldIterator,"end",0,0,Bool::create(self->end()))
}

//---

//! (ctor)
YieldIterator::YieldIterator():
		Object(typeObject),counter(0),active(true) {
	//ctor
}

//! (dtor)
YieldIterator::~YieldIterator() {
	//dtor
}


void YieldIterator::next(Runtime & rt){
	if(!active){
		rt.warn("YieldContext is inactive.");
		return;
	}
	++counter;
	rt.pushContext(runtimeContext.get());
	ObjRef tmp=rt.executeCurrentContext();
	rt.popContext();

	if(rt.getState()==Runtime::STATE_YIELDING){
		myResult=rt.getResult();
		active=true;
		rt.resetState();
	}else if(rt.getState()==Runtime::STATE_RETURNING){
		myResult=rt.getResult();
		active=false;
		rt.resetState();
	}else{
		active=false;
		rt.assertNormalState(this);
	}
}
Object * YieldIterator::key()const{
	return Number::create(getCounter());
}
