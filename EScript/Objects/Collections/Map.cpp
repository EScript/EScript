// Map.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Map.h"
#include "../../Basics.h"
#include "../../StdObjects.h"

#include <string>

namespace EScript{

//! (static)
Type * Map::getTypeObject(){
	static Type * typeObject = new Type(Collection::getTypeObject()); // ---|> Map
	return typeObject;
}

//! initMembers
void Map::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Map new Map( [key,value]* )
	ES_CONSTRUCTOR(typeObject,0,-1, {
		if( (parameter.count()%2)==1 ) rt.warn("Map: Last parameter ignored!");
		Map * a = new Map;
		for(ParameterValues::size_type i = 0;i<parameter.count();i+=2)
			a->setValue(parameter[i],parameter[i+1]);
		return a;
	})

	//! [ESMF] thisObj Map.filter( fn(key,value){return false|true;} [,additionalParameter*] )
	ES_MFUNCTION(typeObject,Map,"filter",1,-1,{
		ParameterValues additionalValues(parameter.count()-1);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+1,parameter.end(),additionalValues.begin());
		thisObj->rt_filter(rt,parameter[0],additionalValues);
		return thisObj;
	})

	//! [ESMF] thisObj Map.unset( key )
	ES_MFUN(typeObject,Map,"unset",1,1,
				(thisObj->unset(parameter[0]),thisEObj))

	//! [ESMF] bool Collection.containsKey(Object)
	ES_MFUN(typeObject,Map,"containsKey",1,1,
				thisObj->getValue(parameter[0].toString())!=nullptr)

	//! [ESMF] thisObj Map.merge( Collection [,bool overwrite = true] )
	ES_MFUN(typeObject,Map,"merge",1,2,
				(thisObj->merge(assertType<Collection>(rt,parameter[0]),parameter[1].toBool(true)),thisEObj))

	//! [ESMF] thisObj swap.swap( Map other )
	ES_MFUN(typeObject,Map,"swap",1,1,
				(thisObj->swap(assertType<Map>(rt,parameter[0])),thisEObj))

}

//---
//! (static) Factory
Map * Map::create(){
	return new Map;
}

//! (static) Factory
Map * Map::create(const std::unordered_map<StringId,Object *> & attr){
	Map * m = create();
	for(const auto & keyValuePair : attr) {
		m->setValue(String::create(keyValuePair.first.toString()), keyValuePair.second->getRefOrCopy());
	}
	return m;
}

//---

void Map::unset(ObjPtr key){
	if(!key.isNull())
		data.erase(key.toString());
}

void Map::merge(Collection * c,bool overwrite/*=true*/){
	if(!c) return;
	ERef<Iterator>  iRef = c->getIterator();
	if(iRef.isNull()) return;

	if(overwrite){
		for( ; !iRef->end(); iRef->next()){
			setValue(iRef->key()->getRefOrCopy(),iRef->value()->getRefOrCopy());
		}
	}else{
		for( ; !iRef->end(); iRef->next()){
			Object * old = getValue(iRef->key());
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
	return key.isNull() ? nullptr : getValue(key.toString());
}


Object * Map::getValue(const std::string & key) {
	container_t::iterator it = data.find(key);
	return it==data.end() ? nullptr : (*it).second.value.get();
}

Object * Map::getKeyObject(const std::string & key) {
	container_t::iterator it = data.find(key);
	return it==data.end() ? nullptr : (*it).second.key.get();
}

//! ---|> Collection
void Map::setValue(ObjPtr key,ObjPtr value) {
	if(key.isNull()) return ;
	std::string ident = key->toString();

	container_t::iterator it = data.find(ident);
	if(it!=data.end()){
		it->second.key = key;
		it->second.value = value;
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
	for(const auto & keyEntryPair : data) {
		const MapEntry & sourceEntry = keyEntryPair.second;
		newMap->setValue(sourceEntry.key->getRefOrCopy(), sourceEntry.value->getRefOrCopy());
	}
	return newMap;
}

void Map::rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues) {
	container_t tempMap;

	ParameterValues parameters(additionalValues.count()+2);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+2);

	for(const auto & keyEntryPair : data) {
		const MapEntry & sourceEntry = keyEntryPair.second;
		parameters.set(0,sourceEntry.key);
		parameters.set(1,sourceEntry.value);
		if( callFunction(runtime,function.get(),parameters).toBool() ){
			tempMap[keyEntryPair.first] = sourceEntry;
		}
	}
	data.swap(tempMap);
}
// ------- MapIterator

//! (ctor)
Map::MapIterator::MapIterator(Map * _map):Iterator(),mapRef(_map) {
	it = mapRef->data.begin();
}


//! ---|> [Iterator]
Object * Map::MapIterator::key() {
	if(end()) return nullptr;
	MapEntry & op=(*it).second;
	return op.key.get();
}

//! ---|> [Iterator]
Object * Map::MapIterator::value() {
	if(end()) return nullptr;
	MapEntry & op=(*it).second;
	return op.value.get();
}

//! ---|> [Iterator]
void Map::MapIterator::next() {
	if(!end())
		++it;
}

//! ---|> [Iterator]
void Map::MapIterator::reset() {
	it = mapRef->data.begin();
}

//! ---|> [Iterator]
bool Map::MapIterator::end() {
	return it==mapRef->data.end();
}

//template<> Map* convertTo<Map*>(Runtime& runtime,ObjPtr src)		{	return assertType<Map>(runtime,src);	}
}
