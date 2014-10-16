// Void.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Void.h"
#include "../../Basics.h"
#include "../../Consts.h"
#include <iostream>

namespace EScript{

//! (static)
Void * Void::get() {
	static ERef<Void> instance = new Void;
	return instance.get();
}

//! (ctor)
Void::Void():Type() {
	Void::addReference(this); // workaround for lost Void in test app.
	
	//! Bool void._checkConstraint( obj )
	ES_FUN(this,Consts::IDENTIFIER_fn_checkConstraint,1,1, parameter[0].castTo<const Void>() !=nullptr) // required for void parameter checks: fn( [Number,void] p1){ }
	
	//ctor
}

//! (dtor)
Void::~Void() {
	std::cout << " LOST VOID\n \a";
	//dtor
}


//! ---|> [Object]
bool Void::rt_isEqual(Runtime &,const ObjPtr & o){
	return o.castTo<const Void>() != nullptr;
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
