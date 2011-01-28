#include "Map.h"
#include "../EScript.h"

#include <string>
#include <sstream>

using namespace EScript;
//---

using std::map;

/*!	[ESMF] Map new Map( [key,value]* )	*/
ES_FUNCTION(esmf_Map_constructor) {
    assertParamCount(runtime,parameter.count(),0);
    if ( (parameter.count()%2)==1 ) runtime.warn("Map: Last parameter ignored!");
    Map * a=new Map(dynamic_cast<Type*>(caller));
    for (ParameterValues::size_type i=0;i<parameter.count();i+=2)
        a->setValue(parameter[i],parameter[i+1]);
    return  a;
}

/*!	[ESMF] self Map.unset( key )	*/
ESF(esmf_Map_unset,1,1,(assertType<Map>(runtime,caller)->unset(parameter[0]),caller))

/*!	[ESMF] self Map.merge( Collection [,bool overwrite=true] )	*/
ESF(esmf_Map_merge,1,2,(assertType<Map>(runtime,caller)->
        merge(assertType<Collection>(runtime,parameter[0]),parameter[1].toBool(true)),caller))

//---
Type* Map::typeObject=NULL;

/*!	initMembers	*/
void Map::init(EScript::Namespace & globals) {
//
    // [Map] ---|> [Collection] ---|> [Object]
    typeObject=new Type(Collection::getTypeObject());
    declareConstant(&globals,getClassName(),typeObject);
    declareFunction(typeObject,"_constructor",esmf_Map_constructor);
    declareFunction(typeObject,"unset",esmf_Map_unset);
    declareFunction(typeObject,"merge",esmf_Map_merge);
}

//---
/*! (static) Factory*/
Map * Map::create(){
	return new Map();
}

/*! (static) Factory */
Map * Map::create(const std::map<identifierId,Object *> & attr){
	Map * m=create();
	for(std::map<identifierId,Object *>::const_iterator it=attr.begin();it!=attr.end();++it)
        m->setValue(String::create( identifierIdToString(it->first) ), it->second->getRefOrCopy());
    return m;
}

//---

/*!	(ctor)	*/
Map::Map(Type * type):Collection(type?type:typeObject) {
    //ctor
}

/*!	(dtor)	*/
Map::~Map() {
    //dtor
}

/*!	*/
void Map::unset(ObjPtr key){
    if(key.isNull()) return;
    m.erase(key.toString());
}


/*!	*/
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

/*!	---|> Collection */
Object * Map::getValue(ObjPtr key) {
    return key.isNull() ? NULL : getValue(key.toString());
}

/*!	*/
Object * Map::getValue(const std::string & key) {
    objectMap_t::iterator it=m.find(key);
    return it==m.end() ? NULL : (*it).second.value.get();
}

/*!	---|> Collection */
void Map::setValue(ObjPtr key,ObjPtr value) {
    if (key.isNull()) return ;
    std::string ident=key->toString();

	objectMap_t::iterator it=m.find(ident);
	if(it!=m.end()){
		it->second.key=key;
		it->second.value=value;
	}else{
		m[ident]=MapEntry(key,value);
	}
}

/*!	---|> Collection */
size_t Map::count() const{
    return m.size();
}

/*!	---|> Collection */
Iterator * Map::getIterator() {
    return new MapIterator(this);
}

/*!	---|> Collection */
void  Map::clear() {
    m.clear();
}

/*!	---|> [Object]	*/
Object * Map::clone()const {
    Map *newMap= new Map(getType());
    for (objectMap_t::const_iterator it=m.begin() ; it!=m.end() ; ++it) {
		const MapEntry & sourceEntry=(*it).second;
		newMap->setValue(sourceEntry.key->getRefOrCopy(),sourceEntry.value->getRefOrCopy());
    }
    return newMap;
}
// ------- MapIterator

/*!	(ctor)	*/
Map::MapIterator::MapIterator(Map * map):Iterator(),mapRef(map) {
    it=mapRef->m.begin();
}

/*!	(dtor)	*/
Map::MapIterator::~MapIterator() {
}

/*!	---|> [Iterator]	*/
Object * Map::MapIterator::key() {
    if (end()) return NULL;
    MapEntry & op=(*it).second;
    return op.key.get();
}

/*!	---|> [Iterator]	*/
Object * Map::MapIterator::value() {
    if (end()) return NULL;
    MapEntry & op=(*it).second;
    return op.value.get();
}

/*!	---|> [Iterator]	*/
void Map::MapIterator::next() {
    if (!end()) {
        ++it;
    }
}

/*!	---|> [Iterator]	*/
void Map::MapIterator::reset() {
    it=mapRef->m.begin();
}

/*!	---|> [Iterator]	*/
bool Map::MapIterator::end() {
    return it==mapRef->m.end();
}
