#include "Namespace.h"

#include "../EScript.h"

using namespace EScript;
//---


/*! (static) */
Type * Namespace::getTypeObject()	{
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}

/*!	initMembers	*/
void Namespace::init(EScript::Namespace & globals) {
    // [Namespace] ---|> [ExtObject] ---|> [Object]
    Type * t=getTypeObject();
    declareConstant(&globals,getClassName(),t);
}

//---

/*!	(ctor)	*/
Namespace::Namespace():
		ExtObject(getTypeObject()) {

    //ctor
}

/*!	(ctor)	*/
Namespace::Namespace(Type * type):
		ExtObject(type) {

    //ctor
}


/*!	(dtor)	*/
Namespace::~Namespace() {
//	std::cout << "~"<<countReferences();
    //dtor
}

/*!	---|> [Object]	*/
Namespace * Namespace::clone() const{
	Namespace * c=new Namespace(getType());
    c->cloneAttributesFrom(this);
    return c;
}

