#include "Type.h"

#include "../EScript.h"

using namespace EScript;
//---
//
/*!	[ESMF] Type new Type( [BaseType=ExtObject] )	*/
ESF(esmf_Type_constructor,0,1,parameter.count() == 0 ?
		new Type(ExtObject::getTypeObject()) :
		new Type(assertType<Type>(runtime,parameter[0])))


/*!	[ESMF] Type Type.getBaseType()	*/
ESF(esmf_Type_getBaseType,0,0, assertType<Type>(runtime,caller)->getBaseType())

/*!	[ESMF] Map Type.getObjAttributes()	*/
ES_FUNCTION(esmf_Type_getObjAttributes){
	assertParamCount(runtime,parameter.count(),0,0);
	std::map<identifierId,Object *>  attrs;
	assertType<Type>(runtime,caller)->getObjAttributes(attrs);
	return Map::create(attrs);
}

/*!	[ESMF] Map Type.getTypeAttributes()	*/
ES_FUNCTION(esmf_Type_getTypeAttributes){
	assertParamCount(runtime,parameter.count(),0,0);
	std::map<identifierId,Object *>  attrs;
	assertType<Type>(runtime,caller)->getTypeAttributes(attrs);
	return Map::create(attrs);
}


//---

/*! (static) */
Type * Type::getTypeObject(){
	static Type * typeObject=NULL;
	if(typeObject==NULL){
		// This object defines the type of all 'Type' objects.
		// It inherits from Object and the type of the object is defined by itself.
		typeObject=new Type(Object::getTypeObject(),NULL);
		typeObject->typeRef=typeObject;
	}
	return typeObject;
}

/*!	initMembers	*/
void Type::init(EScript::Namespace & globals) {
    // [Type] ---|> [Object]
    Type * t=getTypeObject();
    declareConstant(&globals,getClassName(),t);

    declareFunction(t, "_constructor",esmf_Type_constructor);
	declareFunction(t, "getBaseType",esmf_Type_getBaseType);
	declareFunction(t, "getObjAttributes",esmf_Type_getObjAttributes);
	declareFunction(t, "getTypeAttributes",esmf_Type_getTypeAttributes);
}

//---

/*!	(ctor)	*/
Type::Type():
    Object(Type::getTypeObject()),flags(0),baseType(Object::getTypeObject()) {
    //ctor
}

/*!	(ctor)	*/
Type::Type(Type * _baseType):
		Object(Type::getTypeObject()),flags(0),baseType(_baseType) {

	if(getBaseType()!=NULL)
		getBaseType()->initInstanceObjAttributes(this);
    //ctor
}

/*!	(ctor)	*/
Type::Type(Type * _baseType,Type * typeOfType):
		Object(typeOfType),flags(0),baseType(_baseType) {

	if(getBaseType()!=NULL)
		getBaseType()->initInstanceObjAttributes(this);
    //ctor
}

/*!	(dtor)	*/
Type::~Type() {
//	std::cout << "~"<<countReferences();
    //dtor
}

/*!	---|> [Object]	*/
Object * Type::clone() const{
    return new Type(getBaseType(),getType());
}

/*!	*/
bool Type::assignToInheritedAttribute(const identifierId id,ObjPtr val){
	Type * t=this;
	do{
		AttributeMap_t::iterator fIt=t->attr.find(id);
		if( fIt != t->attr.end()){
			(*fIt).second.assign(val.get());
			return true;
		}
		t=t->getBaseType();
	}while(t!=NULL);
	return false;
}

/*!	*/
Object * Type::getInheritedAttribute(const identifierId id)const{
	for(const Type * t=this; t!=NULL; t=t->getBaseType()){
		Object * result=t->getLocalAttribute(id);
		if(result!=NULL)
			return result;
	}
	return NULL;
}


/*!	---|> Object */
Object * Type::getAttribute(const identifierId id){
	// try to find the attribute along the inheritated path...
	Object * result=getInheritedAttribute(id);
	if(result!=NULL)
		return result;

	// try to find the attribute from this type's type.
	return getType()!=NULL ? getType()->getInheritedAttribute(id) : NULL;
}

/*!	*/
Object * Type::getLocalAttribute(const identifierId id)const{
	AttributeMap_t::const_iterator f=attr.find(id);
	return  f!=attr.end() ? f->second.getValue() : NULL ;
}

/*!	---|> Object */
bool Type::setObjAttribute(const identifierId id,ObjPtr val){
	attr[id] = Attribute(val.get(), Attribute::OBJECT);
	setFlag(FLAG_CONTAINS_OBJ_ATTRS,true);
	return true;
}

/*!	---|> Object */
bool Type::assignAttribute(const identifierId id,ObjPtr val){
	// try to assign value along the inheritated path...
	if(assignToInheritedAttribute(id,val))
		return true;

	// try to assign the attribute to this type's type.
	return getType()!=NULL ? getType()->assignAttribute(id,val) : false;
}

/*!		*/
void Type::setTypeAttribute(const identifierId id,ObjPtr val){
	attr[id] = Attribute(val.get(), Attribute::TYPE);
}

/*!		*/
void Type::initInstanceObjAttributes(Object * instance){
	// init member vars of type
	if(getFlag(FLAG_CONTAINS_OBJ_ATTRS)){
		for(AttributeMap_t::iterator it=attr.begin() ; it!=attr.end() ; ++it){
			Attribute & a=it->second;
			if( a.isTypeAttribute() )
				continue;
			Object * value=a.getValue();
			if( value==NULL )
				continue;
			value=value->getRefOrCopy();
			instance->setObjAttribute( it->first, value);
		}
	}
}

/*! */
void Type::getTypeAttributes(std::map<identifierId,Object *> & attrs){
	for(AttributeMap_t::iterator it=attr.begin() ; it!=attr.end() ; ++it){
		if(it->second.isTypeAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

/*! */
void Type::getObjAttributes(std::map<identifierId,Object *> & attrs){
	for(AttributeMap_t::iterator it=attr.begin() ; it!=attr.end() ; ++it){
		if(it->second.isObjAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

/*!	---|> Object	*/
void Type::getAttributes(std::map<identifierId,Object *> & attrs){
	for(AttributeMap_t::iterator it=attr.begin() ; it!=attr.end() ; ++it){
		attrs[it->first] = it->second.getValue();
	}
}

/*!	---|> Object*/
bool Type::isA(Type * type) const {
	if (type == NULL)
		return false;

	for(Type * t=getBaseType();t!=NULL;t=t->getBaseType()){
		if(t==type)
			return true;
	}
	return Object::isA(type);
}
