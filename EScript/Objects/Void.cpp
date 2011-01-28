#include "Void.h"
#include "../EScript.h"

using namespace EScript;

Void * Void::instance=NULL;//new Void();

/*!	initMembers	*/
void Void::init(EScript::Namespace & /*globals*/) {
    instance=new Void;
    Object::addReference(instance);
}

/*!	(static)	*/
Void * Void::get() {
    return instance;
}

/*!	(ctor)	*/
Void::Void():Type() {
    //ctor
}

/*!	(dtor)	*/
Void::~Void() {
    std::cout << " LOST VOID\n \a";
    //dtor
}

///*!	---|> [Object]	*/
//Object * Void::execute(Runtime & /*rt*/) {
//    return this;
//}

/*!	---|> [Object]	*/
bool Void::rt_isEqual(Runtime &,const ObjPtr o){
    return o.toType<Void>() != NULL;
}

/*!	---|> [Object]	*/
Object * Void::clone()const{
    return const_cast<Object *>(dynamic_cast<const Object *>(this));
}

/*!	---|> [Object]	*/
bool Void::toBool()const{
    return false;
}

