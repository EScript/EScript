#include "ExtObject.h"

#include "../EScript.h"

using namespace EScript;
//---
//
/*!	[ESMF] ExtObject new ExtObject()	*/
ESF(esmf_ExtObject_constructor,0,0,	new ExtObject(assertType<Type>(runtime,caller)))


//---

/*! (static) */
Type * ExtObject::getTypeObject(){
	// [ExtObject] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

/*!	(static) initMembers	*/
void ExtObject::init(EScript::Namespace & globals) {
	Type * t=getTypeObject();

    declareConstant(&globals,getClassName(),t);
    declareFunction(t,"_constructor",esmf_ExtObject_constructor);
}

// -----------------------------------------------------------------------------------------------


/*!	(ctor)	*/
ExtObject::ExtObject():
    Object(ExtObject::getTypeObject()),objAttributes(NULL) {
    //ctor
}

/*!	(ctor)	*/
ExtObject::ExtObject(Type * parentType):
    Object(parentType),objAttributes(NULL) {
	if (typeRef.isNull())
		return;

	typeRef->initInstanceObjAttributes(this);
    //ctor
}

/*!	(dtor)	*/
ExtObject::~ExtObject() {
	delete objAttributes;
	objAttributes=NULL;
    //dtor
}

/*!	---|> [Object]	*/
Object * ExtObject::clone() const{
    ExtObject * c=new ExtObject(getType());
    c->cloneAttributesFrom(this);
    return c;
}

// -----------------------------------------------------------------------------------------------
// attributes

/*!	*/
Object * ExtObject::getObjAttribute(const identifierId id)const{
	if(objAttributes!=NULL){
		attributeMap_t::const_iterator f=objAttributes->find(id);
		if( f!=objAttributes->end() ){
			return f->second.get();
		}
	}
	return NULL;
}

/*!	---|> [Object] */
Object * ExtObject::getAttribute(const identifierId id){
	if(objAttributes!=NULL){
		attributeMap_t::const_iterator f=objAttributes->find(id);
		if( f!=objAttributes->end() )
			return f->second.get();
	}
	return Object::getAttribute(id);
}

/*!	---|> [Object] */
bool ExtObject::setObjAttribute(const identifierId id,ObjPtr val){
	if(objAttributes == NULL)
		objAttributes = new attributeMap_t();
	(*objAttributes)[id] = val;
	return true;
}

/*!	---|> [Object] */
bool ExtObject::assignAttribute(const identifierId id,ObjPtr val){
	return assignObjAttribute(id,val) ? true :  Object::assignAttribute(id,val);
}

/*!	*/
bool ExtObject::assignObjAttribute(const identifierId id,ObjPtr val){
	if(objAttributes!=NULL){
		attributeMap_t::iterator it=objAttributes->find(id);
		if(it!=objAttributes->end()){
			it->second=val;
			return true;
		}
	}
	return false;
}

/*!	*/
void ExtObject::cloneAttributesFrom(const ExtObject * obj) {
	if(obj->objAttributes==NULL)
		return;

	for(attributeMap_t::iterator it=obj->objAttributes->begin() ; it!=obj->objAttributes->end() ; ++it){
		ObjPtr attr=it->second;
		setObjAttribute(it->first, attr->getRefOrCopy());
	}
}

/*!	---|> Object	*/
void ExtObject::getAttributes(std::map<identifierId,Object *> & attrs){
	if(objAttributes!=NULL){
		for(attributeMap_t::iterator it=objAttributes->begin() ; it!=objAttributes->end() ; ++it){
			attrs[it->first] = it->second.get();
		}
	}
}

