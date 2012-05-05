// Map.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Map.h"
#include "../../EScript.h"

#include <string>

using namespace EScript;
//---

using std::map;

//---

//! (static)
Type * Map::getTypeObject(){
	// [Map] ---|> [Collection]
	static Type * typeObject=new Type(Collection::getTypeObject());
	return typeObject;
}

//! initMembers
void Map::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Map new Map( [key,value]* )
	ES_FUNCTION_DECLARE(typeObject,"_constructor",0,-1, {
		if ( (parameter.count()%2)==1 ) runtime.warn("Map: Last parameter ignored!");
		Map * a=new Map();
		for (ParameterValues::size_type i=0;i<parameter.count();i+=2)
			a->setValue(parameter[i],parameter[i+1]);
		return a;
	})

	//! [ESMF] self Map.filter( fn(key,value){return false|true;} [,additionalParameter*] )
	ES_MFUNCTION_DECLARE(typeObject,Map,"filter",1,-1,{
		ParameterValues additionalValues(parameter.count()-1);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+1,parameter.end(),additionalValues.begin());
		self->rt_filter(runtime,parameter[0],additionalValues);
		return self;
	})

	//! [ESMF] self Map.unset( key )
	ESMF_DECLARE(typeObject,Map,"unset",1,1,
				(self->unset(parameter[0]),caller))

	//! [ESMF] bool Collection.containsKey(Object)
	ESMF_DECLARE(typeObject,Map,"containsKey",1,1,
				Bool::create(self->getValue(parameter[0].toString())!=NULL))

	//! [ESMF] self Map.merge( Collection [,bool overwrite=true] )
	ESMF_DECLARE(typeObject,Map,"merge",1,2,
				(self->merge(assertType<Collection>(runtime,parameter[0]),parameter[1].toBool(true)),caller))

	//! [ESMF] self swap.swap( Map other )
	ESMF_DECLARE(typeObject,Map,"swap",1,1,
				(self->swap(assertType<Map>(runtime,parameter[0])),caller))

}

//---
/*! (static) Factory*/
Map * Map::create(){
	return new Map();
}

//! (static) Factory
Map * Map::create(const std::unordered_map<StringId,Object *> & attr){
	Map * m=create();
	for(std::unordered_map<StringId,Object *>::const_iterator it=attr.begin();it!=attr.end();++it)
		m->setValue(String::create( (it->first).toString() ), it->second->getRefOrCopy());
	return m;
}

//---

//! (ctor)
Map::Map(Type * type):Collection(type?type:getTypeObject()) {
	//ctor
}

//! (dtor)
Map::~Map() {
	//dtor
}

void Map::unset(ObjPtr key){
	if(!key.isNull())
		data.erase(key.toString());
}

void Map::merge(Collection * c,bool overwrite/*=true*/){
	if(!c) return;
	ERef<Iterator>  iRef=c->getIterator();
	if (iRef.isNull()) return;

	if(overwrite){
		for( ; !iRef->end(); iRef->next()){
			setValue(iRef->key()->getRefOrCopy(),iRef->value()->getRefOrCopy());
		}
	}else{
		for( ; !iRef->end(); iRef->next()){
			Object * old=getValue(iRef->key());
			if(!old){
				setValue(iRef->key()->getRefOrCopy(),iRef->value()->getRefOrCopy());
			}
		}
	}
}

void Map::swap(Map * other){
	data.swap(other->data);
}

//! ---|> Collection
Object * Map::getValue(ObjPtr key) {
	return key.isNull() ? NULL : getValue(key.toString());
}


Object * Map::getValue(const std::string & key) {
	container_t::iterator it=data.find(key);
	return it==data.end() ? NULL : (*it).second.value.get();
}

Object * Map::getKeyObject(const std::string & key) {
	container_t::iterator it=data.find(key);
	return it==data.end() ? NULL : (*it).second.key.get();
}

//! ---|> Collection
void Map::setValue(ObjPtr key,ObjPtr value) {
	if (key.isNull()) return ;
	std::string ident=key->toString();

	container_t::iterator it=data.find(ident);
	if(it!=data.end()){
		it->second.key=key;
		it->second.value=value;
	}else{
		data[ident]=MapEntry(key,value);
	}
}

//! ---|> Collection
size_t Map::count() const{
	return data.size();
}

//! ---|> Collection
Map::MapIterator * Map::getIterator() {
	return new MapIterator(this);
}

//! ---|> Collection
void  Map::clear() {
	data.clear();
}

//! ---|> [Object]
Object * Map::clone()const {
	Map *newMap= new Map(getType());
	for (container_t::const_iterator it=data.begin() ; it!=data.end() ; ++it) {
		const MapEntry & sourceEntry=(*it).second;
		newMap->setValue(sourceEntry.key->getRefOrCopy(),sourceEntry.value->getRefOrCopy());
	}
	return newMap;
}

void Map::rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues) {
	container_t tempMap;

	ParameterValues parameters(additionalValues.count()+2);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+2);

	for (container_t::const_iterator it=data.begin() ; it!=data.end() ; ++it) {
		const MapEntry & sourceEntry=(*it).second;
		parameters.set(0,sourceEntry.key);
		parameters.set(1,sourceEntry.value);
		ObjRef resultRef=callFunction(runtime,function.get(),parameters);
		if( resultRef.toBool() ){
			tempMap[ it->first ] = sourceEntry;
		}
	}
	data.swap(tempMap);
}
// ------- MapIterator

//! (ctor)
Map::MapIterator::MapIterator(Map * _map):Iterator(),mapRef(_map) {
	it=mapRef->data.begin();
}

//! (dtor)
Map::MapIterator::~MapIterator() {
}

//! ---|> [Iterator]
Object * Map::MapIterator::key() {
	if (end()) return NULL;
	MapEntry & op=(*it).second;
	return op.key.get();
}

//! ---|> [Iterator]
Object * Map::MapIterator::value() {
	if (end()) return NULL;
	MapEntry & op=(*it).second;
	return op.value.get();
}

//! ---|> [Iterator]
void Map::MapIterator::next() {
	if (!end())
		++it;
}

//! ---|> [Iterator]
void Map::MapIterator::reset() {
	it=mapRef->data.begin();
}

//! ---|> [Iterator]
bool Map::MapIterator::end() {
	return it==mapRef->data.end();
}
