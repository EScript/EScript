// Bool.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Bool.h"

#include "../../EScript.h"

#include <iostream>
#include <stack>

namespace EScript{

//! (static)
Type * Bool::getTypeObject(){
	// [Bool] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
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
	ESF_DECLARE(typeObject,"&",1,1,
				Bool::create( caller->toBool() & parameter[0]->toBool()))

	//! [ESMF] Bool Bool | ((Bool)obj)
	ESF_DECLARE(typeObject,"|",1,1,
				Bool::create( caller->toBool() | parameter[0]->toBool()))

	//! [ESMF] Bool Bool ^ ((Bool)obj)
	ESF_DECLARE(typeObject,"^",1,1,
				Bool::create( caller->toBool() ^ parameter[0]->toBool()))

//	//! [ESMF] Bool !Bool
//	ES_FUNCTION_DECLARE(typeObject,"!_pre",0,0,{
//						std::cout << " ????? ";
//						return Bool::create(! caller->toBool());
//						})

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
static std::stack<Bool *> boolPool;

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
	if(o->getType()!=getTypeObject()){
		delete o;
		std::cout << "Found diff BoolType\n";
	}else{
		boolPool.push(o);
	}
}

//---

Bool::Bool(bool _value,Type * type):
		Object(type?type:getTypeObject()),value(_value) {
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
