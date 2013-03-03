// Collection.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Collection.h"
#include "../../Basics.h"
#include "../../StdObjects.h"
#include "../../Consts.h"

#include <algorithm>

namespace EScript{

//! (static)
Type * Collection::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void Collection::init(EScript::Namespace & globals) {

	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Object Collection[key]
	ES_MFUN(typeObject,Collection,"_get",1,1,thisObj->getValue(parameter[0]))

	//! [ESMF] thisObj Collection[key] = value
	ES_MFUN(typeObject,Collection,"_set",2,2,
				(thisObj->setValue(parameter[0],parameter[1]),thisEObj))

	//! [ESMF] Object Collection.get(key [,default value] )
	ES_MFUNCTION(typeObject,Collection,"get",1,2,{
		ObjPtr result = thisObj->getValue(parameter[0].get());
		return (parameter.count()>1 && result.isNull())?parameter[1].get():result.get();
	})

	//! [ESMF] thisObj Collection.set(key,value)
	ES_MFUN(typeObject,Collection,"set",2,2,
				(thisObj->setValue(parameter[0],parameter[1]),thisEObj))

	//! [ESMF] Number Collection.size() \deprecated
	ES_MFUN(typeObject,Collection, "size",0,0, static_cast<uint32_t>(thisObj->count()))

	//! [ESMF] Number Collection.count()
	ES_MFUN(typeObject,Collection, "count",0,0, static_cast<uint32_t>(thisObj->count()))

	//! [ESMF] Bool Collection.empty()
	ES_MFUN(typeObject,Collection,"empty",0,0,thisObj->count()==0 )

	//! [ESMF] thisObj Collection.clear()
	ES_MFUN(typeObject,Collection,"clear",0,0,(thisObj->clear(),thisEObj))

	//! [ESMF] Iterator Collection.getIterator()
	ES_MFUN(typeObject,Collection,"getIterator",0,0,thisObj->getIterator())

	//! [ESMF] Collection Collection.map(function[, AdditionalValues*])
	ES_MFUNCTION(typeObject,Collection,"map",1,-1,{
		ParameterValues additionalValues(parameter.count()-1);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+1,parameter.end(),additionalValues.begin());
		return thisObj->rt_map(rt,parameter[0],additionalValues);
	})

	//! [ESMF] Object Collection.max()
	ES_MFUN(typeObject,Collection,"max",0,0,
				thisObj->rt_extract(rt,Consts::IDENTIFIER_fn_greater,true))

	//! [ESMF] Object Collection.min()
	ES_MFUN(typeObject,Collection, "min",0,0,
				thisObj->rt_extract(rt,Consts::IDENTIFIER_fn_greater,false))

	//! [ESMF] bool Collection.contains(Object)
	ES_MFUN(typeObject,Collection,"contains",1,1,
				thisObj->rt_contains(rt,parameter[0]))

	//! [ESMF] KEY Collection.findValue( VALUE )
	ES_MFUN(typeObject,Collection, "findValue",1,1,
				thisObj->rt_findValue(rt,parameter[0]))

	//! [ESMF] KEY Collection.reduce(fn(runningVar,key,value){ return ...}[,initialValue = void,[,additionalParameters]])
	ES_MFUNCTION(typeObject,Collection,"reduce",1,-1,{
		ParameterValues additionalValues(parameter.count()>2 ? parameter.count()-2 : 0);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+2,parameter.end(),additionalValues.begin());
		return thisObj->rt_reduce(rt,parameter[0],parameter[1],additionalValues);
	})
}

//---


//! ---o
Object * Collection::getValue(ObjPtr /*key*/) {
	return nullptr;
}

//! ---o
void Collection::setValue(ObjPtr /*key*/,ObjPtr /*value*/) {}

//! ---o
size_t Collection::count()const {
	return 0;
}

//! ---o
void Collection::clear() {
}

//! ---o
Iterator * Collection::getIterator() {
	return nullptr;
}
/**
 * ---o
 * ['a','b','c'].findValue('c') =>
 *   'c'.'=='('a') , 'c'.'=='('b') , 'c'.'=='('c') -> return 2
 */
Object * Collection::rt_findValue(Runtime & runtime,ObjPtr subject){
	for(ERef<Iterator> it = getIterator(); !it->end() ; it->next()){
		ObjRef key = it->key();
		ObjRef value = it->value();
		if(subject->isEqual(runtime,value)){
			return key.detachAndDecrease();
		}
	}
	return nullptr;
}
/**
 * ---o
 * ['a','b','c'].contains('c') =>
 *   'c'.'=='('a') , 'c'.'=='('b') , 'c'.'=='('c') -> return true
 */
bool Collection::rt_contains(Runtime & runtime,ObjPtr subject){
	for(ERef<Iterator> it = getIterator(); !it->end() ; it->next()){
		ObjRef key = it->key();
		ObjRef value = it->value();
		if(subject->isEqual(runtime,value)){
			return true;
		}
	}
	return false;
}
/**
 * ---o
 * [1,2,3].reduce(fn(sum,key,value){return sum+value;},0) => 6
 * [1,2,3].reduce(fn(sum,key,value,x){return sum+(value*x);},0,0.5) => 3
 */
Object * Collection::rt_reduce(Runtime & runtime,ObjPtr function,ObjPtr initialValue, const ParameterValues & additionalValues){
	ObjRef runningVar = initialValue.isNull() ? Void::get() : initialValue;

	ParameterValues parameters(additionalValues.count()+3);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+3);

	for(ERef<Iterator> it = getIterator(); !it->end() ; it->next()){
		ObjRef key = it->key();
		ObjRef value = it->value();
//		std::cout << "running var: ["<<runningVar.toString()<<"]\n";
		parameters.set(0,runningVar);
		parameters.set(1,key);
		parameters.set(2,value);
		runningVar = callFunction(runtime,function.get(),parameters); 
	}
	return runningVar.detachAndDecrease();
}

//! ---|> Object
bool Collection::rt_isEqual(Runtime &runtime,const ObjPtr & other){
	Collection * c = other.toType<Collection>();
	if(c==nullptr || count()!=c->count() ) return false;

	bool b = true;
	for(ERef<Iterator> it = getIterator(); !it->end() ;  it->next()){
		ObjRef key = it->key();
		ObjRef value = it->value();
		if(value.isNull())
			value = Void::get();

		ObjRef value2 = c->getValue(key.get());
		b = value->isEqual(runtime,value2);

		if(!b) break;
	}
	return b;
}
/**
 * ---o
 * [1,2,3].map(fn(key,value){return sum+value;}) => [1,3,5]
 * [1,2,3].map(fn(key,value,s){return s+sum+value;},"foo") => ["foo1","foo3","foo5"]
 */
Object * Collection::rt_map(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues){
	// Create new, empty Collection
	ObjRef obj = callMemberFunction(runtime,getType(),Consts::IDENTIFIER_fn_constructor,ParameterValues());
	ERef<Collection> newCollectionRef = obj.toType<Collection>();
	if(newCollectionRef.isNull()){
		runtime.setException("Collection.map(..) No Contructor found!");
		return nullptr;
	}
	obj = nullptr;

	ParameterValues parameters(additionalValues.count()+2);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+2);


	for( ERef<Iterator> it = getIterator(); ! it->end() ; it->next()){
		ObjRef key = it->key();
		ObjRef value = it->value();
		parameters.set(0,key);
		parameters.set(1,value);
		ObjRef newValue = runtime.executeFunction(function.get(),nullptr,parameters);
		if(!newValue.isNull())
			newCollectionRef->setValue(key.get(),newValue.get());
	}
	return newCollectionRef.detachAndDecrease();
}

//! ---o ???
Object * Collection::rt_extract(Runtime & runtime,StringId functionId,bool decision/*=true*/){
	ERef<Iterator> it = getIterator();

	ObjRef currentValue;
	while(! it->end()) {
		ObjRef value = it->value();

		if(currentValue.isNull()) {
			currentValue = value;
		} else {
			if(callMemberFunction(runtime,value.get(),functionId,ParameterValues(currentValue.get())).toBool()==decision)
				currentValue = value;
		}
		it->next();
	}
	// detach object from "currentValue" without deleting it.
	return currentValue.detachAndDecrease();
}
}//namespace EScript
