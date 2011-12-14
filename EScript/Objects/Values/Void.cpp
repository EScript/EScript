// Void.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Void.h"

using namespace EScript;

//! (static)
Void * Void::get() {
	static ERef<Void> instance=new Void;
	return instance.get();
}

//! (ctor)
Void::Void():Type() {
	Void::addReference(this); // workaround for lost Void in test app.
	//ctor
}

//! (dtor)
Void::~Void() {
	std::cout << " LOST VOID\n \a";
	//dtor
}

//! ---|> [Object]
bool Void::rt_isEqual(Runtime &,const ObjPtr o){
	return o.toType<Void>() != NULL;
}

//! ---|> [Object]
Object * Void::clone()const{
	return get();
}

//! ---|> [Object]
bool Void::toBool()const{
	return false;
}
