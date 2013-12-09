// Namespace.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Namespace.h"

#include "../Basics.h"

namespace EScript{

//! (static)
Type * Namespace::getTypeObject(){
	static Type * typeObject = new Type(ExtObject::getTypeObject()); // ---|> ExtObject
	return typeObject;
}

//! initMembers
void Namespace::init(EScript::Namespace & globals) {
	// [Namespace] ---|> [ExtObject] ---|> [Object]
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Namespace new Namespace
	ES_CTOR(typeObject,0,0, new Namespace)
}

//---

//! ---|> [Object]
Namespace * Namespace::clone() const{
	Namespace * c = new Namespace(getType());
	c->cloneAttributesFrom(this);
	return c;
}
}
