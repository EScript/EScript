// Iterator.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Iterator.h"

#include "../EScript.h"

#include "Values/Bool.h"

using namespace EScript;

//! (static)
Type * Iterator::getTypeObject(){
	// [Iterator] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

//! initMembers
void Iterator::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! Bool Iterator.end()
	ESMF_DECLARE(typeObject,Iterator,"end",0,0,Bool::create(self->end()))

	//! Object Iterator.key()
	ESMF_DECLARE(typeObject,Iterator,"key",0,0,self->key())

	//! self Iterator.next()
	ESMF_DECLARE(typeObject,Iterator,"next",0,0,(self->next(),caller))

	//! self Iterator.reset()
	ESMF_DECLARE(typeObject,Iterator,"reset",0,0,(self->reset(),caller))

	//! Object Iterator.value()
	ESMF_DECLARE(typeObject,Iterator,"value",0,0,self->value())
}

//---

//! (ctor)
Iterator::Iterator(Type * type):Object(type?type:getTypeObject()) {
	//ctor
}

//! (dtor)
Iterator::~Iterator() {
	//dtor
}

//! ---o
Object * Iterator::key() {
	return NULL;
}

//! ---o
Object * Iterator::value() {
	return NULL;
}

//! ---o
void Iterator::reset() {
}

//! ---o
void Iterator::next() {
}

//! ---o
bool Iterator::end() {
	return true;
}
