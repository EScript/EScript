// ExtObject.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ExtObject.h"

#include "../EScript.h"

using namespace EScript;

//! (static)
Type * ExtObject::getTypeObject(){
	// [ExtObject] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

//! (static) initMembers
void ExtObject::init(EScript::Namespace & globals) {
	Type * typeObject=getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());
	
	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESF] ExtObject new ExtObject( [Map objAttributes] )
	ES_MFUNCTION_DECLARE(typeObject,Type,"_constructor",0,1,{
		ERef<ExtObject> result(new ExtObject(self));
		if(parameter.count()>0){
			Map * m=assertType<Map>(runtime,parameter[0]);
			for(Map::const_iterator it=m->begin();it!=m->end();++it)
				result->setAttribute(it->first, Attribute(it->second.value));
		}
		return result.detachAndDecrease();
	})

}

// -----------------------------------------------------------------------------------------------

//! (static) factory
ExtObject * ExtObject::create(){
	return new ExtObject();
}


//! (ctor)
ExtObject::ExtObject():
		Object(ExtObject::getTypeObject()) {
	//ctor
}

//! (ctor)
ExtObject::ExtObject(Type * type):
		Object(type) {
	if (typeRef.isNull())
		return;

	typeRef->copyObjAttributesTo(this);
	//ctor
}

//! (dtor)
ExtObject::~ExtObject() {
	//dtor
}

//! ---|> [Object]
Object * ExtObject::clone() const{
	ExtObject * c=new ExtObject(getType());
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
Attribute * ExtObject::_accessAttribute(const StringId id,bool localOnly){
	Attribute * attr = objAttributes.accessAttribute(id);
	
	if( attr==NULL && !localOnly && getType()!=NULL){
		attr = getType()->findTypeAttribute(id);
	}
	return attr;
}

//! ---|> [Object]
bool ExtObject::setAttribute(const StringId id,const Attribute & attr){
	objAttributes.setAttribute(id,attr);
	return true;
}

void ExtObject::cloneAttributesFrom(const ExtObject * obj) {
	objAttributes.cloneAttributesFrom(obj->objAttributes);
}

//! ---|> Object
void ExtObject::collectLocalAttributes(std::map<StringId,Object *> & attrs){
	objAttributes.collectAttributes(attrs);
}
