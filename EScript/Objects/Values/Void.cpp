// Void.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Void.h"

namespace EScript{

//! (static)
Void * Void::get() {
	static ERef<Void> instance = new Void;
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
bool Void::rt_isEqual(Runtime &,const ObjPtr & o){
	return o.castTo<Void>() != nullptr;
}

//! ---|> [Object]
Object * Void::clone()const{
	return get();
}

//! ---|> [Object]
bool Void::toBool()const{
	return false;
}

//! ---|> [Object]
std::string Void::toString()const{
	return "void";
}
}
