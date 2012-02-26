// Type.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Type.h"

#include "../EScript.h"

namespace EScript{

//! (static)
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

//! initMembers
void Type::init(EScript::Namespace & globals) {
	// [Type] ---|> [Object]
	Type * typeObject=getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Type new Type( [BaseType=ExtObject] )
	ES_FUNCTION_DECLARE(typeObject,"_constructor",0,1,{
		Type * baseType = parameter.count() == 0 ? ExtObject::getTypeObject() : assertType<Type>(runtime,parameter[0]);
		if(!baseType->allowsUserInheritance()){
			runtime.setException("Basetype '"+baseType->toString()+"' does not allow user inheritance.");
			return NULL;
		}
		Type * newType = new Type(baseType);
		newType->allowUserInheritance(true); // user defined Types allow user inheritance per default.
		return newType;
	})


	//! [ESMF] Type Type.getBaseType()
	ESMF_DECLARE(typeObject,const Type,"getBaseType",0,0, self->getBaseType())


	// attrMap_t is declared outside of the getObjAttributes declaration as it otherwise leads to a strange
	// preprocessor error on gcc.
	typedef std::map<StringId, Object *> attrMap_t;

	//! [ESMF] Map Type.getObjAttributes()
	ES_MFUNCTION_DECLARE(typeObject,const Type,"getObjAttributes",0,0,{
		attrMap_t attrs;
		self->collectObjAttributes(attrs);
		return Map::create(attrs);
	})

	//! [ESMF] Map Type.getTypeAttributes()
	ES_MFUNCTION_DECLARE(typeObject,const Type,"getTypeAttributes",0,0,{
		attrMap_t attrs;
		self->collectTypeAttributes(attrs);
		return Map::create(attrs);
	})
}

//---

//! (ctor)
Type::Type():
	Object(Type::getTypeObject()),flags(0),baseType(Object::getTypeObject()) {
	//ctor
}

//! (ctor)
Type::Type(Type * _baseType):
		Object(Type::getTypeObject()),flags(0),baseType(_baseType) {

	if(getBaseType()!=NULL)
		getBaseType()->copyObjAttributesTo(this);
	//ctor
}

//! (ctor)
Type::Type(Type * _baseType,Type * typeOfType):
		Object(typeOfType),flags(0),baseType(_baseType) {

	if(getBaseType()!=NULL)
		getBaseType()->copyObjAttributesTo(this);
	//ctor
}

//! (dtor)
Type::~Type() {
	//dtor
}

//! ---|> [Object]
Object * Type::clone() const{
	return new Type(getBaseType(),getType());
}

static const char * typeAttrErrorHint =
	"This may be a result of: Adding object attributes to a Type AFTER inheriting from that Type, "
	"adding object attributes to a Type AFTER creating instances of that Type, "
	"or adding object attributes to a Type whose instances cannot store object attributes. ";


Attribute * Type::findTypeAttribute(const StringId id){
	Type * t=this;
	do{
		Attribute * attr = t->attributes.accessAttribute(id);
		if( attr != NULL ){
			if( attr->isObjAttribute() ){
				std::string message = "(findTypeAttribute) type-attribute expected but object-attribute found. ('";
				message += id.toString() + "')\n" + typeAttrErrorHint;
				throw new Exception(message);
			}
			return attr;
		}
		t=t->getBaseType();
	}while(t!=NULL);
	return NULL;
}


//! ---|> Object
Attribute * Type::_accessAttribute(const StringId id,bool localOnly){
	// is local attribute?
	Attribute * attr = attributes.accessAttribute(id);
	if(attr!=NULL || localOnly)
		return attr;

	// try to find the attribute along the inherited path...
	if(getBaseType()!=NULL){
		attr = getBaseType()->findTypeAttribute(id);
		if(attr!=NULL)
			return attr;
	}
	// try to find the attribute from this type's type.
	return getType()!=NULL ? getType()->findTypeAttribute(id) : NULL;
}

//! ---|> Object
bool Type::setAttribute(const StringId id,const Attribute & attr){
	attributes.setAttribute(id,attr);
	if(attr.isObjAttribute())
		setFlag(FLAG_CONTAINS_OBJ_ATTRS,true);
	return true;
}

void Type::copyObjAttributesTo(Object * instance){
	// init member vars of type
	if(getFlag(FLAG_CONTAINS_OBJ_ATTRS)){
		for(AttributeContainer::iterator it=attributes.begin() ; it!=attributes.end() ; ++it){
			const Attribute & a=it->second;
			if( a.isNull() || a.isTypeAttribute() )
				continue;
			instance->setAttribute( it->first, Attribute(a.getValue()->getRefOrCopy(),a.getFlags()));
		}
	}
}

void Type::collectTypeAttributes(std::map<StringId,Object *> & attrs)const{
	for(AttributeContainer::const_iterator it=attributes.begin() ; it!=attributes.end() ; ++it){
		if(it->second.isTypeAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

void Type::collectObjAttributes(std::map<StringId,Object *> & attrs)const{
	for(AttributeContainer::const_iterator it=attributes.begin() ; it!=attributes.end() ; ++it){
		if(it->second.isObjAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

//! ---|> Object
void Type::collectLocalAttributes(std::map<StringId,Object *> & attrs){
	for(AttributeContainer::iterator it=attributes.begin() ; it!=attributes.end() ; ++it){
		attrs[it->first] = it->second.getValue();
	}
}

//! ---|> Object
bool Type::isA(Type * type) const {
	if (type == NULL)
		return false;

	for(Type * t=getBaseType();t!=NULL;t=t->getBaseType()){
		if(t==type)
			return true;
	}
	return Object::isA(type);
}

}
