// Bool.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Bool.h"

#include "../../Basics.h"

#include <iostream>
#include <stack>

namespace EScript{

template<> Bool * assertType<Bool>(Runtime & runtime, const ObjPtr & obj) {
	if(obj.isNull()||obj->_getInternalTypeId()!=_TypeIds::TYPE_BOOL) 
		_Internals::assertType_throwError(runtime, obj, Bool::getClassName());
	return static_cast<Bool*>(obj.get());
}

//! (static)
Type * Bool::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void Bool::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//- Operators

	//! Bool Bool & ((Bool)obj)
	ES_FUN(typeObject,"&",1,1,
				( thisEObj->toBool() & parameter[0].toBool())>0 )

	//! [ESMF] Bool Bool | ((Bool)obj)
	ES_FUN(typeObject,"|",1,1,
				( thisEObj->toBool() | parameter[0].toBool())>0 )

	//! [ESMF] Bool Bool ^ ((Bool)obj)
	ES_FUN(typeObject,"^",1,1,
				( thisEObj->toBool() ^ parameter[0].toBool())>0 )

//	//! [ESMF] Bool !Bool
//	ES_FUNCTION(typeObject,"!_pre",0,0,{
//						std::cout << " ????? ";
//						return Bool::create(! thisEObj->toBool());
//						})

	//! [ESMF] Bool |= Bool
	ES_MFUN(typeObject,Bool,"|=",1,1,
				(thisObj->setValue(thisEObj->toBool() | parameter[0].toBool()) ,thisEObj))

	//! [ESMF] Bool &= Bool
	ES_MFUN(typeObject,Bool,"&=",1,1,
				(thisObj->setValue(thisEObj->toBool() & parameter[0].toBool()) ,thisEObj))


	//- Comparisons

	//! [ESMF] Bool Bool > ((Bool)obj)
	ES_FUN(typeObject,">",1,1,
				( thisEObj->toBool() > parameter[0].toBool()))

	//! [ESMF] Bool Bool < ((Bool)obj)
	ES_FUN(typeObject,"<",1,1,
				( thisEObj->toBool() < parameter[0].toBool()))


}
//----
static std::stack<Bool *> pool;

Bool * Bool::create(bool value){
//static int count = 0;
//std::cout << ++count<<" "; //2807  // (1396)
	#ifdef ES_DEBUG_MEMORY
	return new Bool(value);
	#endif
	if(pool.empty()){
		for(int i = 0;i<32;++i){
			pool.push(new Bool(false));
		}
		return create(value);
	}else{
		Bool * o = pool.top();
		pool.pop();
		o->value = value;
//        std::cout << ".";
		return o;
	}

}
void Bool::release(Bool * o){
	#ifdef ES_DEBUG_MEMORY
	delete o;
	return;
	#endif
	if(o->getType()!=getTypeObject()){
		delete o;
		std::cout << "Found diff BoolType\n";
	}else{
		pool.push(o);
	}
}

//---

} // namespace EScript
