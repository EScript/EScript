// YieldIterator.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "YieldIterator.h"

#include "../Basics.h"
#include "../StdObjects.h"

namespace EScript{

//! (static)
Type * YieldIterator::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void YieldIterator::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! Object YieldIterator.key()
	ES_MFUN(typeObject,YieldIterator,"key",0,0,thisObj->key())

	//! Object YieldIterator.value()
	ES_MFUN(typeObject,YieldIterator,"value",0,0,thisObj->value())

	//! thisObj YieldIterator.next()
	ES_MFUN(typeObject,YieldIterator,"next",0,0,(thisObj->next(rt),thisEObj))

	//! bool YieldIterator.end()
	ES_MFUN(typeObject,YieldIterator,"end",0,0,thisObj->end())
}

//---

void YieldIterator::next(Runtime & rt){
	++counter;
	rt.yieldNext( *this );
}

Object * YieldIterator::key()const{
	return create(getCounter());
}
}
