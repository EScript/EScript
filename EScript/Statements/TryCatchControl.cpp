// TryCatchControl.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "TryCatchControl.h"

#include "Block.h"
#include "../Runtime/Runtime.h"

#include <sstream>

using namespace EScript;

//! (ctor)
TryCatchControl::TryCatchControl(Object * tryBlock,Block * catchBlock,identifierId _varNameId):
		tryBlockRef(tryBlock),catchBlockRef(catchBlock),varNameId(_varNameId) {
	//ctor
}

//! (dtor)
TryCatchControl::~TryCatchControl() {
	//dtor
}

//! ---|> [Object]
std::string TryCatchControl::toString()const {
	std::ostringstream sprinter;
	sprinter << "try "<<tryBlockRef.toString()<<catchBlockRef.toString();
	return sprinter.str();
}

//! ---|> [Object]
Object * TryCatchControl::execute(Runtime & rt) {
	ObjRef resultRef=NULL;

	if (!tryBlockRef.isNull()){
		resultRef=rt.executeObj(tryBlockRef.get());
	}
	if(rt.getState()==Runtime::STATE_EXCEPTION){
		ObjRef exceptionRef=rt.getResult();
		rt.resetState();
		if (!catchBlockRef.isNull()){
			resultRef=rt.executeCatchBlock(catchBlockRef.get(),varNameId,exceptionRef.get());
		}
	}
	return resultRef.detachAndDecrease();
}
