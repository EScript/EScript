// StreamBase.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StreamBase.h"
#include "../../EScript.h"

#include <algorithm>

namespace EScript{

//! (static)
Type * StreamBase::getTypeObject()	{
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}

//! initMembers
void StreamBase::init(EScript::Namespace & globals) {

	// StreamBase ---|> [ExtObject]
	Type * typeObject=getTypeObject();
	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Object StreamBase[key]
	ESF_DECLARE(typeObject,"_constructor",0,0,new StreamBase(dynamic_cast<Type*>(caller)))

}

//---

//! (ctor)
StreamBase::StreamBase(Type * type):ExtObject(type?type:getTypeObject()) {
	//ctor
}

//! (dtor)
StreamBase::~StreamBase() {
	//dtor
}

}//namespace EScript
