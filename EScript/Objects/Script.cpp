#include "Script.h"

#include "../EScript.h"
#include "Number.h"
#include "NumberRef.h"
#include "Bool.h"
#include "Void.h"
#include "String.h"
#include "Collection.h"
#include "Array.h"
#include "Map.h"
#include "Iterator.h"
#include "Exception.h"
#include "Delegate.h"
#include "ReferenceObject.h"
#include "Type.h"
#include "Namespace.h"


using namespace EScript;
ERef<Namespace> Script::SGLOBALS;

// -----------------------

/*!	static	*/
void Script::init() {
    if (!SGLOBALS.isNull()) return;

    // Create GLOBALS
    // \note SGLOBALS must not inherit from Object, as it must not inherit any member
    // 	 Attributes [BUG20100618]
    SGLOBALS=new Namespace();
//    SGLOBALS=new Type(); // \todo change to namespace

    Object::init(*SGLOBALS.get());
    Type::init(*SGLOBALS.get());
    ExtObject::init(*SGLOBALS.get());

    Void::init(*SGLOBALS.get());
    Number::init(*SGLOBALS.get());
    NumberRef::init(*SGLOBALS.get());
    Bool::init(*SGLOBALS.get());
    String::init(*SGLOBALS.get());

    Collection::init(*SGLOBALS.get());
    Iterator::init(*SGLOBALS.get());
    Array::init(*SGLOBALS.get());
    Map::init(*SGLOBALS.get());
//    Pointer::init(*SGLOBALS.get());
    Exception::init(*SGLOBALS.get());
    Delegate::init(*SGLOBALS.get());
    Namespace::init(*SGLOBALS.get());

	declareConstant(SGLOBALS.get(),"VERSION",String::create(ES_VERSION));
	declareConstant(SGLOBALS.get(),"SGLOBALS",SGLOBALS.get());

}

/*!	(ctor)	*/
Script::Script() {
    rootBlock=new Block();
    //ctor
}

/*!	(dtor)	*/
Script::~Script() {
    //dtor
}

/*!	*/
Block * Script::getRootBlock()const {
    return rootBlock.get();
}

/*!	*/
void Script::loadScript(std::string filename) throw (Exception *) {
    EScript::loadScriptFile(filename,getRootBlock());
}

/*!	---|> [Object]	*/
Object * Script::execute(Runtime & rt) {
    ObjRef result=rt.executeObj(getRootBlock());
    if(rt.getState()==Runtime::STATE_EXIT){
        result=rt.getResult();
    }else if(rt.getState()==Runtime::STATE_EXCEPTION){
        result=NULL;
        throw(rt.getResult());
    }
    return result.detachAndDecrease();
}
