// Function.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Function.h"
#include "../Identifier.h"
#include "../../EScript.h"

using namespace EScript;

//! (static)
Type * Function::getTypeObject()	{
	//! Function ---> Object
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

//! (static) initMembers
void Function::init(EScript::Namespace & globals) {
	Type * typeObject=getTypeObject();
	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Number|false Function.getMaxParamCount()
	ES_MFUNCTION_DECLARE(typeObject,Function,"getMaxParamCount",0,0,{
		if(self->getMaxParamCount()<0 )
			return Bool::create(false);
		return Number::create(self->getMaxParamCount());
	})

	//! [ESMF] Number Function.getMinParamCount()
	ESMF_DECLARE(typeObject,Function,"getMinParamCount",0,0, Number::create(self->getMinParamCount()))

	//! [ESMF] Identifier Function.getOriginalName()
	ESMF_DECLARE(typeObject,Function,"getOriginalName",0,0, Identifier::create(self->getOriginalName()))

	//! [ESMF] (experimental) Number Function._getCallCounter()
	ESMF_DECLARE(typeObject,Function,"_getCallCounter",0,0, Number::create(self->getCallCounter()))
}

//! (ctor)
Function::Function(identifierId _originalName, int _minParamCount, int _maxParamCount, functionPtr _fnptr) :
		Object(getTypeObject()),fnptr(_fnptr),minParamCount(_minParamCount),maxParamCount(_maxParamCount),
		originalName(_originalName),callCounter(0) {
}

//! (ctor)
Function::Function(functionPtr _fnptr) :
		Object(getTypeObject()),fnptr(_fnptr),minParamCount(0),maxParamCount(-1),originalName(0),callCounter(0) {
}

//! (ctor)
Function::~Function() {
}
