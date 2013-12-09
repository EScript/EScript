// Iterator.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Iterator.h"
#include "../Basics.h"

namespace EScript{

//! (static)
Type * Iterator::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void Iterator::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! Bool Iterator.end()
	ES_MFUN(typeObject,Iterator,"end",0,0,thisObj->end())

	//! Object Iterator.key()
	ES_MFUN(typeObject,Iterator,"key",0,0,thisObj->key())

	//! thisObj Iterator.next()
	ES_MFUN(typeObject,Iterator,"next",0,0,(thisObj->next(),thisEObj))

	//! thisObj Iterator.reset()
	ES_MFUN(typeObject,Iterator,"reset",0,0,(thisObj->reset(),thisEObj))

	//! Object Iterator.value()
	ES_MFUN(typeObject,Iterator,"value",0,0,thisObj->value())
}

}
