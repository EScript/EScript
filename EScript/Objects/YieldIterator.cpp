// YieldIterator.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "YieldIterator.h"

#include "../EScript.h"

using namespace EScript;
//---
//! (static)
Type * YieldIterator::getTypeObject(){
	// [Iterator] ---|> [Object]
	static Type * typeObject = new Type(Object::getTypeObject());
	return typeObject;
}

//! initMembers
void YieldIterator::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

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



void YieldIterator::next(Runtime & rt){
	++counter;
	rt.yieldNext( *this );
}

Object * YieldIterator::key()const{
	return Number::create(getCounter());
}
