// ExtObject.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ExtObject.h"

#include "../Basics.h"
#include "../StdObjects.h"

namespace EScript{

//! (static)
Type * ExtObject::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! (static) initMembers
void ExtObject::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESF] ExtObject new ExtObject( [Map objAttributes] )
	ES_CONSTRUCTOR(typeObject,0,1,{
		ERef<ExtObject> result(new ExtObject(thisType));
		if(parameter.count()>0){
			Map * m = assertType<Map>(rt,parameter[0]);
			for(const auto & keyValuePair : *m) {
				result->setAttribute(keyValuePair.first, Attribute(keyValuePair.second.value));
			}
		}
		return result.detachAndDecrease();
	})

}

// -----------------------------------------------------------------------------------------------

//! (static) factory
ExtObject * ExtObject::create(){
	return new ExtObject;
}


//! (ctor)
ExtObject::ExtObject() : Object(ExtObject::getTypeObject()) {
	//ctor
}

ExtObject::ExtObject(const ExtObject & other): Object(other.getType()){
//	if(typeRef.isNotNull())
//		typeRef->copyObjAttributesTo(this);
	cloneAttributesFrom(&other);
}

//! (ctor)
ExtObject::ExtObject(Type * type) : Object(type) {
	if(typeRef.isNotNull())
		typeRef->copyObjAttributesTo(this);
	//ctor
}


//! ---|> [Object]
Object * ExtObject::clone() const{
	ExtObject * c = new ExtObject(getType());
	c->cloneAttributesFrom(this);
	return c;
}

// -----------------------------------------------------------------------------------------------
// attributes

//! ---|> [Object]
void ExtObject::_initAttributes(Runtime & rt){
	objAttributes.initAttributes(rt);
}


//! ---|> [Object]
Attribute * ExtObject::_accessAttribute(const StringId & id,bool localOnly){
	Attribute * attr = objAttributes.accessAttribute(id);

	if( attr==nullptr && !localOnly && getType()!=nullptr){
		attr = getType()->findTypeAttribute(id);
	}
	return attr;
}

//! ---|> [Object]
bool ExtObject::setAttribute(const StringId & id,const Attribute & attr){
	objAttributes.setAttribute(id,attr);
	return true;
}

void ExtObject::cloneAttributesFrom(const ExtObject * obj) {
	objAttributes.cloneAttributesFrom(obj->objAttributes);
}

//! ---|> Object
void ExtObject::collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs){
	objAttributes.collectAttributes(attrs);
}
}
