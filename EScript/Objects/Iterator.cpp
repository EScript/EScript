#include "Iterator.h"

#include "../EScript.h"

#include "Bool.h"
#include "Function.h"

using namespace EScript;
//---
/*! Object Iterator.key() */
ESF(esmf_Iterator_key,0,0,assertType<Iterator>(runtime,caller)->key())

/*! Object Iterator.value() */
ESF(esmf_Iterator_value,0,0,assertType<Iterator>(runtime,caller)->value())

/*! self Iterator.reset() */
ESF(esmf_Iterator_reset,0,0,(assertType<Iterator>(runtime,caller)->reset(),caller))

/*! self Iterator.next() */
ESF(esmf_Iterator_next,0,0,(assertType<Iterator>(runtime,caller)->next(),caller))

/*! Bool Iterator.end() */
ESF(esmf_Iterator_end,0,0,Bool::create(assertType<Iterator>(runtime,caller)->end()))

//---
Type* Iterator::typeObject=NULL;

/*!	initMembers	*/
void Iterator::init(EScript::Namespace & globals) {
    // Iterator ---|> [Object]
    typeObject=new Type(Object::getTypeObject());
    declareConstant(&globals,getClassName(),typeObject);

    declareFunction(typeObject,"key",esmf_Iterator_key);
    declareFunction(typeObject,"value",esmf_Iterator_value);
    declareFunction(typeObject,"reset",esmf_Iterator_reset);
    declareFunction(typeObject,"next",esmf_Iterator_next);
    declareFunction(typeObject,"end",esmf_Iterator_end);
}

//---

/*!	(ctor)	*/
Iterator::Iterator(Type * type):Object(type?type:typeObject) {
    //ctor
}

/*!	(dtor)	*/
Iterator::~Iterator() {
    //dtor
}

/*!	---o	*/
Object * Iterator::key() {
    return NULL;
}

/*!	---o	*/
Object * Iterator::value() {
    return NULL;
}

/*!	---o	*/
void Iterator::reset() {
}

/*!	---o	*/
void Iterator::next() {
}

/*!	---o	*/
bool Iterator::end() {
    return true;
}

