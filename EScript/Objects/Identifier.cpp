// Identifier.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Identifier.h"

namespace EScript{

//! (static)
Identifier * Identifier::create( identifierId id){
	return new Identifier(id);
}

//! (static)
Identifier * Identifier::create( const std::string & s){
	return new Identifier(stringToIdentifierId(s));
}

//! (ctor)
Identifier::Identifier(const identifierId &_id):
		Object(),id(_id) {
	//ctor
}

//! (dtor)
Identifier::~Identifier() {
	//dtor
}

//! ---|> [Object]
std::string Identifier::toString()const {
	return identifierIdToString(getId());
}

}
