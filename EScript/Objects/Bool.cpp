// Bool.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Bool.h"

#include "../EScript.h"
#include "Function.h"
#include "../Runtime/Runtime.h"

#include <sstream>

namespace EScript{

Type* Bool::typeObject=NULL;

//! initMembers
void Bool::init(EScript::Namespace & globals) {
//
	// Bool ---|> [Object]
	typeObject=new Type(Object::getTypeObject());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);
	declareConstant(&globals,getClassName(),typeObject);

	//- Operators

	//! [ESMF] Bool Bool + ((Bool)obj)
	ESF_DECLARE(typeObject,"+",1,1,
				Bool::create( caller->toBool() + parameter[0]->toBool()))

	//! Bool Bool - ((Bool)obj)
	ESF_DECLARE(typeObject,"-",1,1,
				Bool::create( caller->toBool() - parameter[0]->toBool()))

	//! Bool Bool & ((Bool)obj)
	ESF_DECLARE(typeObject,"&",1,1,
				Bool::create( caller->toBool() & parameter[0]->toBool()))

	//! [ESMF] Bool Bool | ((Bool)obj)
	ESF_DECLARE(typeObject,"|",1,1,
				Bool::create( caller->toBool() | parameter[0]->toBool()))

	//! [ESMF] Bool Bool ^ ((Bool)obj)
	ESF_DECLARE(typeObject,"^",1,1,
				Bool::create( caller->toBool() ^ parameter[0]->toBool()))

	//! [ESMF] Bool !Bool
	ESF_DECLARE(typeObject,"!_pre",0,0,
				Bool::create(! caller->toBool()))

	//! [ESMF] Bool |= Bool
	ESMF_DECLARE(typeObject,Bool,"|=",1,1,
				(self->setValue(caller->toBool() | parameter[0].toBool()) ,caller))

	//! [ESMF] Bool &= Bool
	ESMF_DECLARE(typeObject,Bool,"&=",1,1,
				(self->setValue(caller->toBool() & parameter[0].toBool()) ,caller))


	//- Comparisons

	//! [ESMF] Bool Bool > ((Bool)obj)
	ESF_DECLARE(typeObject,">",1,1,
				Bool::create( caller->toBool() > parameter[0]->toBool()))

	//! [ESMF] Bool Bool < ((Bool)obj)
	ESF_DECLARE(typeObject,"<",1,1,
				Bool::create( caller->toBool() < parameter[0]->toBool()))


}
//----
std::stack<Bool *> boolPool=std::stack<Bool *>();

Bool * Bool::create(bool value){
	#ifdef ES_DEBUG_MEMORY
	return new Bool(value);
	#endif
	if(boolPool.empty()){
		for(int i=0;i<32;i++){
			boolPool.push(new Bool(false));
		}
		return create(value);
	}else{
		Bool * o=boolPool.top();
		boolPool.pop();
		o->value=value;
//        std::cout << ".";
		return o;
	}

}
void Bool::release(Bool * o){
	#ifdef ES_DEBUG_MEMORY
	delete o;
	return;
	#endif
	if(o->getType()!=typeObject){
		delete o;
		std::cout << "Found diff BoolType\n";
	}else{
		boolPool.push(o);
	}
}

//---

Bool::Bool(bool _value,Type * type):
		Object(type?type:typeObject),value(_value) {
	//ctor
}

Bool::~Bool() {
	//dtor
}

//! ---|> [Object]
Object * Bool::clone() const {
	return Bool::create(value);
}

//! ---|> [Object]
std::string Bool::toString()const {
	return value?"true":"false";
}

//! ---|> [Object]
double Bool::toDouble()const {
	return value?1:0;
}

//! ---|> [Object]
bool Bool::toBool()const {
	return value;
}

//! ---|> [Object]
bool Bool::rt_isEqual(Runtime &,const ObjPtr o){
	return  value==o.toBool(false);
}
} // namespace EScript
