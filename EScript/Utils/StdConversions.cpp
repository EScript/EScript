// StdConversions.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "StdConversions.h"

#include "../Basics.h"
#include "../StdObjects.h"

namespace EScript{

template<>
double convertTo<double>(Runtime &runtime,ObjPtr src){
	if(src){
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
