// ConversionBasics.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StdConversions.h"

#include "../Basics.h"
#include "../StdObjects.h"

namespace EScript{

template<>
double convertTo<double>(Runtime &runtime,ObjPtr src){
	if(src.isNotNull()){
		if(src->_getInternalTypeId()==_TypeIds::TYPE_NUMBER){
			return **static_cast<Number*>(src.get());
		}else if(src->_getInternalTypeId()==_TypeIds::TYPE_STRING){
			return src->toDouble();
		}else{
			runtime.warn("Converting "+  src.toDbgString() +" to Number.");
//			assertType_throwError(runtime,src,Number::getClassName());
			return src->toDouble();
		}
	}
	return 0.0;
//	return src.toDouble();
}

} 