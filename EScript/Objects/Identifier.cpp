// Identifier.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Identifier.h"
#include "../Basics.h"

namespace EScript{

//! (static)
Type * Identifier::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! (static) initMembers
void Identifier::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] Identifier new Identifier( string )
	ES_CTOR(typeObject,1,1,Identifier::create(parameter[0].toString()))

}

//! (static)
Identifier * Identifier::create( StringId id){
	return new Identifier(id);
}

//! (static)
Identifier * Identifier::create( const std::string & s){
	return new Identifier(s);
}

//! (ctor)
Identifier::Identifier(const StringId &_id):
		Object(getTypeObject()),id(_id) {
	//ctor
}


//! ---|> [Object]
bool Identifier::rt_isEqual(Runtime &,const ObjPtr & o){
	Identifier * other = o.castTo<Identifier>();
	return other == nullptr ? false : other->getId() == this->getId();
}

}
