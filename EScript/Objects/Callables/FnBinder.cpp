// FnBinder.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "FnBinder.h"

#include "../../Basics.h"
#include "../Collections/Array.h"

namespace EScript{

//! (static)
Type * FnBinder::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//----
static std::stack<FnBinder *> pool;

FnBinder * FnBinder::create(ObjPtr object,ObjPtr function){
	#ifdef ES_DEBUG_MEMORY
	return new FnBinder(object,function);
	#endif
	if(pool.empty()){
		for(int i = 0;i<32;++i){
			pool.push(new FnBinder(nullptr,nullptr));
		}
		return create(object,function);
	}else{
		FnBinder * o = pool.top();
		pool.pop();
		o->myObjectRef = object;
		o->functionRef = function;
//        std::cout << ".";
		return o;
	}
}
FnBinder * FnBinder::create(ObjPtr object,ObjPtr function,std::vector<ObjRef>&&params){
	FnBinder* binder = create(object,function);
	binder->boundParameters = std::move(params);
	return binder;
}

void FnBinder::release(FnBinder * o){
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
void FnBinder::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESF] FnBinder new FnBinder(object,function, _boundParams...)
	ES_CONSTRUCTOR(typeObject,2,-1,{
		std::vector<ObjRef> params;
		for(size_t i=2;i<parameter.count();++i)
			params.emplace_back(parameter[i]);
		return FnBinder::create(parameter[0],parameter[1],std::move(params));
	})
	//! [ESF] FnBinder FnBinder.bindParameters( fun, p... )
	ES_FUNCTION(typeObject,"bindParameters",1,-1,{
		std::vector<ObjRef> params;
		for(size_t i=1;i<parameter.count();++i)
			params.emplace_back(parameter[i]);
		return FnBinder::create(nullptr,parameter[0],std::move(params));
	})
	// bindParameters( fun, p...)
	// bind( fun, obj, ... )

	//!	[ESMF] Object FnBinder.getObject()
	ES_MFUN(typeObject,const FnBinder,"getObject",0,0,thisObj->getObject())

	//!	[ESMF] Object FnBinder.getFunction()
	ES_MFUN(typeObject,const FnBinder,"getFunction",0,0,thisObj->getFunction())

	//!	[ESMF] Array FnBinder.getBoundParameters()
	ES_MFUN(typeObject,const FnBinder,"getBoundParameters",0,0,Array::create(thisObj->getBoundParameters()))

	//!	[ESMF] Bool FnBinder.isObjectBound()
	ES_MFUN(typeObject,const FnBinder,"isObjectBound",0,0,thisObj->getObject()!=nullptr)
}

//---

//! (ctor)
FnBinder::FnBinder(ObjPtr object,ObjPtr function):
		Object(getTypeObject()),myObjectRef(object.get()),functionRef(function.get()) {
	//ctor
}


//! ---|> [Object]
FnBinder * FnBinder::clone() const{
	return new FnBinder(getObject(),getFunction());
}

//! ---|> [Object]
bool FnBinder::rt_isEqual(Runtime &,const ObjPtr & o){
	FnBinder * d = o.castTo<FnBinder>();
	return (d!=nullptr) &&   d->getObject()==getObject() && d->getFunction()==getFunction();
}

//! ---|> [Object]
std::string FnBinder::toDbgString()const {
	return std::string('('+myObjectRef.toString("?")+"->"+(functionRef.isNull() ? "?" : functionRef->toDbgString())+')');
}
}
