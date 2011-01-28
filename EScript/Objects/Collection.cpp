#include "Collection.h"
#include "../EScript.h"

#include "../Utils/StringUtils.h"

#include <string>
#include <sstream>

namespace EScript{

using std::string;

namespace EMembers{

/*!
    \name EClass: Collection ---|> Object
    \see EScript::Collection
    @{
*/


/*!     [ESMF] Bool Collection.empty()  */
ESF(esmf_Collection_empty,0,0,Bool::create( assertType<Collection>(runtime,caller)->count()==0 ))

/*!     [ESMF] Object Collection.get((Object)key[,defaultValue])    */
ES_FUNCTION(esmf_Collection_get){
    assertParamCount(runtime,parameter.count(),1,2);
    ObjPtr result=assertType<Collection>(runtime,caller)->getValue(parameter[0]);
    return (parameter.count()>1 && result.isNull())?parameter[1].get():result.get();
}

/*!	[ESMF] self Collection.set((Object)key,(Object)value)	*/
ESF(esmf_Collection_set,2,2, (assertType<Collection>(runtime,caller)->setValue(parameter[0],parameter[1]),caller))

/*!	[ESMF] self Collection.clear()	*/
ESF(esmf_Collection_clear,0,0, (assertType<Collection>(runtime,caller)->clear(),caller))

/*!	[ESMF] Iterator Collection.getIterator()	*/
ESF(esmf_Collection_getIterator,0,0,assertType<Collection>(runtime,caller)->getIterator())

/*!	[ESMF] Collection Collection.map(function[, ...])	*/
ESF(esmf_Collection_map,1,1,assertType<Collection>(runtime,caller)->rt_map(runtime,parameter[0]))
//
///*!	[ESMF] Bool Collection == (Collection)Obj	*/
//ESF(esmf_Collection_equal,1,1,Bool::create(assertType<Collection>(runtime,caller)->rt_isEqual(runtime,parameter[0])))

/*!	[ESMF] Object Collection.max()	*/
ESF(esmf_Collection_max,0,0,assertType<Collection>(runtime,caller)->rt_extract(runtime,Consts::IDENTIFIER_fn_greater,true))

/*!	[ESMF] bool Collection.contains(Object)	*/
ESF(esmf_Collection_contains,1,1,Bool::create(assertType<Collection>(runtime,caller)->rt_contains(runtime,parameter[0])))


/*!	[ESMF] KEY Collection.reduce(fn(runningVar,key,value){ return ...}[,initialValue=void])	*/
ESMF(Collection,esmf_Collection_reduce,1,2,
		self->rt_reduce(runtime,parameter[0],parameter[1]))

/*! @} */
}// namespace EMembers
//---

/*! (static) */
Type * Collection::getTypeObject()	{
	static Type * typeObject=new Type(ExtObject::getTypeObject());
	return typeObject;
}

/*!	initMembers	*/
void Collection::init(EScript::Namespace & globals) {
//
    // Collection ---|> [ExtObject]
    Type * t=getTypeObject();
    declareConstant(&globals,getClassName(),t);
//
    using namespace EMembers;


	/*!	[ESMF] Number Collection.size()	*/ // depreceated!?
	ESMF_DECLARE(t,Collection, "size",		0,0,	Number::create( self->count() ))

	/*!	[ESMF] Number Collection.count()	*/
	ESMF_DECLARE(t,Collection, "count",		0,0,	Number::create( self->count() ))

    declareFunction(t,Consts::IDENTIFIER_fn_get,	esmf_Collection_get); // _get []
    declareFunction(t,Consts::IDENTIFIER_fn_set,	esmf_Collection_set); // _set []=
    declareFunction(t,"get",						esmf_Collection_get); // @depreceated ??
    declareFunction(t,"set",						esmf_Collection_set);// @depreceated ??
    declareFunction(t,"clear",						esmf_Collection_clear);
    declareFunction(t,Consts::IDENTIFIER_fn_getIterator,esmf_Collection_getIterator);
    declareFunction(t,"map",						esmf_Collection_map);
//    declareFunction(t,Consts::IDENTIFIER_fn_equal,	esmf_Collection_equal);
    declareFunction(t,"empty",						esmf_Collection_empty);
    declareFunction(t,"max",						esmf_Collection_max);
//    declareFunction(t,"min",						esmf_Collection_min);
    declareFunction(t,"contains",					esmf_Collection_contains); // -> containsValue?
//    declareFunction(t,"findValue",					esmf_Collection_findValue);
//    declareFunction(t,"find",						esmf_Collection_findValue); // @deprecated
    declareFunction(t,"reduce",						esmf_Collection_reduce);


	/*!	[ESMF] Object Collection.min()	*/
	ESMF_DECLARE(t,Collection, "min",		0,0,	self->rt_extract(runtime,Consts::IDENTIFIER_fn_greater,false))

	/*!	[ESMF] KEY Collection.findValue(Subject)	*/
	ESMF_DECLARE(t,Collection, "findValue",	1,1,	self->rt_findValue(runtime,parameter[0]))


    // replace, split, filter
    // avg, median

}

//---

Collection::Collection(Type * type):ExtObject(type?type:getTypeObject()) {
    //ctor
}

Collection::~Collection() {
    //dtor
}

/*!	---o	*/
Object * Collection::getValue(ObjPtr /*key*/) {
    return NULL;
}

/*!	---o	*/
void Collection::setValue(ObjPtr /*key*/,ObjPtr /*value*/) {}

/*!	---o	*/
size_t Collection::count()const {
    return 0;
}

/*!	---o	*/
void Collection::clear() {
}

/*!	---o	*/
Iterator * Collection::getIterator() {
    return NULL;
}
/**
 * ---o
 * ['a','b','c'].findValue('c') =>
 *   'c'.'=='('a') , 'c'.'=='('b') , 'c'.'=='('c') -> return 2
 */
Object * Collection::rt_findValue(Runtime & runtime,ObjPtr subject){
    for(ERef<Iterator> it=getIterator(); !it->end() ; it->next()){
        ObjRef key=it->key();
        ObjRef value=it->value();
        if(subject->isEqual(runtime,value)){
            return key.detachAndDecrease();
        }
    }
    return NULL;
}
/**
 * ---o
 * ['a','b','c'].contains('c') =>
 *   'c'.'=='('a') , 'c'.'=='('b') , 'c'.'=='('c') -> return true
 */
bool Collection::rt_contains(Runtime & runtime,ObjPtr subject){
    for(ERef<Iterator> it=getIterator(); !it->end() ; it->next()){
        ObjRef key=it->key();
        ObjRef value=it->value();
        if(subject->isEqual(runtime,value)){
            return true;
        }
    }
    return false;
}
/**
 * ---o
 * [1,2,3].reduce(fn(sum,key,value){return sum+value;},0) => 6
 */
Object * Collection::rt_reduce(Runtime & runtime,ObjPtr function,ObjPtr initialValue){
    ObjRef runningVar= initialValue.isNull() ? Void::get() : initialValue;

    for(ERef<Iterator> it=getIterator(); !it->end() ; it->next()){
        ObjRef key=it->key();
        ObjRef value=it->value();
        runningVar=callFunction(runtime,function.get(),ParameterValues(runningVar,key,value));
    }
    return runningVar.detachAndDecrease();
}

/*!	---|> Object	*/
bool Collection::rt_isEqual(Runtime &runtime,const ObjPtr other){
    Collection * c=other.toType<Collection>();
    if (c==NULL || count()!=c->count() ) return false;

    bool b=true;
    for(ERef<Iterator> it=getIterator(); !it->end() ;  it->next()){
        ObjRef key=it->key();
        ObjRef value=it->value();
        if (value.isNull())
            value=Void::get();

        ObjRef value2=c->getValue(key.get());
        b=value->isEqual(runtime,value2);

        if (!b) break;
    }
    return b;
}
/**
 * ---o
 * [1,2,3].map(fn(key,value){return sum+value;}) => [1,3,5]
 */
Object * Collection::rt_map(Runtime & runtime,ObjPtr function){
    // Create new, empty Collection
    Object * obj=callMemberFunction(runtime,this,Consts::IDENTIFIER_fn_constructor,ParameterValues());
    ERef<Collection> newCollectionRef=dynamic_cast<Collection*>(obj);
    if(newCollectionRef.isNull()){
        runtime.error("Collection.map(..) No Contructor found!");
        return NULL;
    }
    for( ERef<Iterator> it=getIterator(); ! it->end() ; it->next()){
        ObjRef key=it->key();
        ObjRef value=it->value();

        ObjRef newValue=runtime.executeFunction(function.get(),NULL,ParameterValues(key,value));
        if(!newValue.isNull())
            newCollectionRef->setValue(key.get(),newValue.get());
    }
    return newCollectionRef.detachAndDecrease();
}

/*!	---o ???	*/
Object * Collection::rt_extract(Runtime & runtime,identifierId functionId,bool decision/*=true*/){
    ERef<Iterator> it=getIterator();

    ObjRef currentValue=NULL;
    while (! it->end()) {
        ObjRef value=it->value();

        if (currentValue.isNull()) {
            currentValue=value;
        } else {
            ObjRef result=callMemberFunction(runtime,value.get(),functionId,ParameterValues(currentValue.get()));
            if(result.toBool()==decision)
                currentValue=value;
        }
        it->next();
    }
    // detach object from "currentValue" without deleting it.
    return currentValue.detachAndDecrease();
}
}//namespace EScript
