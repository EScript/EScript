// Function.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "Function.h"
#include "../../Basics.h"
#include "../Identifier.h"

namespace EScript{

//! (static)
Type * Function::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! (static) initMembers
void Function::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Number|false Function.getMaxParamCount()
	ES_MFUNCTION(typeObject,Function,"getMaxParamCount",0,0,{
		if(thisObj->getMaxParamCount()<0 )
			return false;
		return thisObj->getMaxParamCount();
	})

	//! [ESMF] Number Function.getMinParamCount()
	ES_MFUN(typeObject,Function,"getMinParamCount",0,0, thisObj->getMinParamCount())

	//! [ESMF] Identifier Function.getOriginalName()
	ES_MFUN(typeObject,Function,"getOriginalName",0,0, thisObj->getOriginalName())

	//! [ESMF] (experimental) Number Function._getCallCounter()
	ES_MFUN(typeObject,Function,"_getCallCounter",0,0, thisObj->getCallCounter())
}

//! (ctor)
Function::Function(StringId _originalName, int _minParamCount, int _maxParamCount, functionPtr _fnptr) :
		Object(getTypeObject()),fnptr(_fnptr),minParamCount(_minParamCount),maxParamCount(_maxParamCount),
		originalName(_originalName),callCounter(0) {
}

//! (ctor)
Function::Function(functionPtr _fnptr) :
		Object(getTypeObject()),fnptr(_fnptr),minParamCount(0),maxParamCount(-1),originalName(0),callCounter(0) {
}
}
