// Identifier.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Identifier.h"
#include "../EScript.h"

namespace EScript{

//! (static)
Type * Identifier::getTypeObject(){
	// [Identifier] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

//! (static) initMembers
void Identifier::init(EScript::Namespace & globals) {
	Type * typeObject=getTypeObject();
	initPrintableName(typeObject,getClassName());
	
	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] Identifier new Identifier( string )
	ESF_DECLARE(typeObject,"_constructor",1,1,Identifier::create(parameter[0].toString()))

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

//! (dtor)
Identifier::~Identifier() {
	//dtor
}

//! ---|> [Object]
std::string Identifier::toString()const {
	return getId().toString();
}

//! ---|> [Object]
bool Identifier::rt_isEqual(Runtime &,const ObjPtr o){
	Identifier * other = o.toType<Identifier>();
	return other == NULL ? false : other->getId() == this->getId();
}

//! ---|> [Object]
Identifier * Identifier::clone()const{
	return Identifier::create(this->getId());
}

}
