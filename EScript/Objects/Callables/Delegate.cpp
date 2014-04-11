// Delegate.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Delegate.h"

#include "../../Basics.h"
#include "../Collections/Array.h"

namespace EScript{

//! (static)
Type * Delegate::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//----
static std::stack<Delegate *> pool;

Delegate * Delegate::create(ObjPtr object,ObjPtr function){
	#ifdef ES_DEBUG_MEMORY
	return new Delegate(object,function);
	#endif
	if(pool.empty()){
		for(int i = 0;i<32;++i){
			pool.push(new Delegate(nullptr,nullptr));
		}
		return create(object,function);
	}else{
		Delegate * o = pool.top();
		pool.pop();
		o->myObjectRef = object;
		o->functionRef = function;
//        std::cout << ".";
		return o;
	}
}
Delegate * Delegate::create(ObjPtr object,ObjPtr function,std::vector<ObjRef>&&params){
	Delegate* delegate = create(object,function);
	delegate->boundParameters = std::move(params);
	return delegate;
}

void Delegate::release(Delegate * o){
	#ifdef ES_DEBUG_MEMORY
	delete o;
	return;
	#endif
	o->myObjectRef = nullptr;
	o->functionRef = nullptr;
	o->boundParameters.clear();
	pool.push(o);
}

//! initMembers
void Delegate::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESF] Delegate new Delegate(object,function, _boundParams...)
	ES_CONSTRUCTOR(typeObject,2,-1,{
		std::vector<ObjRef> params;
		for(size_t i=2;i<parameter.count();++i)
			params.emplace_back(parameter[i]);
		return Delegate::create(parameter[0],parameter[1],std::move(params));
	})
	//! [ESF] Delegate Delegate.bindParameters( fun, p... )
	ES_FUNCTION(typeObject,"bindParameters",1,-1,{
		std::vector<ObjRef> params;
		for(size_t i=1;i<parameter.count();++i)
			params.emplace_back(parameter[i]);
		return Delegate::create(nullptr,parameter[0],std::move(params));
	})
	// bindParameters( fun, p...)
	// bind( fun, obj, ... )

	//!	[ESMF] Object Delegate.getObject()
	ES_MFUN(typeObject,const Delegate,"getObject",0,0,thisObj->getObject())

	//!	[ESMF] Object Delegate.getFunction()
	ES_MFUN(typeObject,const Delegate,"getFunction",0,0,thisObj->getFunction())

	//!	[ESMF] Array Delegate.getBoundParameters()
	ES_MFUN(typeObject,const Delegate,"getBoundParameters",0,0,Array::create(thisObj->getBoundParameters()))

	//!	[ESMF] Bool Delegate.isObjectBound()
	ES_MFUN(typeObject,const Delegate,"isObjectBound",0,0,thisObj->getObject()!=nullptr)
}

//---

//! (ctor)
Delegate::Delegate(ObjPtr object,ObjPtr function):
		Object(getTypeObject()),myObjectRef(object.get()),functionRef(function.get()) {
	//ctor
}


//! ---|> [Object]
Delegate * Delegate::clone() const{
	return new Delegate(getObject(),getFunction());
}

//! ---|> [Object]
bool Delegate::rt_isEqual(Runtime &,const ObjPtr & o){
	Delegate * d = o.castTo<Delegate>();
	return (d!=nullptr) &&   d->getObject()==getObject() && d->getFunction()==getFunction();
}

//! ---|> [Object]
std::string Delegate::toDbgString()const {
	return std::string('('+myObjectRef.toString("?")+"->"+(functionRef.isNull() ? "?" : functionRef->toDbgString())+')');
}
}
