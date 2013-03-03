// Type.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Type.h"

#include "../Basics.h"
#include "../StdObjects.h"
#include "Exception.h"

namespace EScript{

//! (static)
Type * Type::getTypeObject(){
	struct factory{ // use factory for static one time initialization
		Type * operator()(){
			// This object defines the type of all 'Type' objects.
			// It inherits from Object and the type of the object is defined by itthisObj.
			Type * typeObject = new Type(Object::getTypeObject(),nullptr);
			typeObject->typeRef = typeObject;
			return typeObject;
		}
	};

	static Type * typeObject = factory()();
	return typeObject;
}

//! initMembers
void Type::init(EScript::Namespace & globals) {
	// [Type] ---|> [Object]
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Type new Type( [BaseType = ExtObject] )
	ES_CONSTRUCTOR(typeObject,0,1,{
		Type * baseType = parameter.count() == 0 ? ExtObject::getTypeObject() : assertType<Type>(rt,parameter[0]);
		if(!baseType->allowsUserInheritance()){
			rt.setException("Basetype '"+baseType->toString()+"' does not allow user inheritance.");
			return nullptr;
		}
		Type * newType = new Type(baseType);
		newType->allowUserInheritance(true); // user defined Types allow user inheritance per default.
		return newType;
	})


	//! [ESMF] Type Type.getBaseType()
	ES_MFUN(typeObject,const Type,"getBaseType",0,0, thisObj->getBaseType())


	// attrMap_t is declared outside of the getObjAttributes declaration as it otherwise leads to a strange
	// preprocessor error on gcc.
	typedef std::unordered_map<StringId, Object *> attrMap_t;

	//! [ESMF] Map Type.getObjAttributes()
	ES_MFUNCTION(typeObject,const Type,"getObjAttributes",0,0,{
		attrMap_t attrs;
		thisObj->collectObjAttributes(attrs);
		return Map::create(attrs);
	})

	//! [ESMF] Map Type.getTypeAttributes()
	ES_MFUNCTION(typeObject,const Type,"getTypeAttributes",0,0,{
		attrMap_t attrs;
		thisObj->collectTypeAttributes(attrs);
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

	if(getBaseType()!=nullptr)
		getBaseType()->copyObjAttributesTo(this);
	//ctor
}

//! (ctor)
Type::Type(Type * _baseType,Type * typeOfType):
		Object(typeOfType),flags(0),baseType(_baseType) {

	if(getBaseType()!=nullptr)
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


Attribute * Type::findTypeAttribute(const StringId & id){
	Type * t = this;
	do{
		Attribute * attr = t->attributes.accessAttribute(id);
		if( attr != nullptr ){
			if( attr->isObjAttribute() ){
				std::string message = "(findTypeAttribute) type-attribute expected but object-attribute found. ('";
				message += id.toString() + "')\n" + typeAttrErrorHint;
				throw new Exception(message);
			}
			return attr;
		}
		t = t->getBaseType();
	}while(t!=nullptr);
	return nullptr;
}


//! ---|> Object
Attribute * Type::_accessAttribute(const StringId & id,bool localOnly){
	// is local attribute?
	Attribute * attr = attributes.accessAttribute(id);
	if(attr!=nullptr || localOnly)
		return attr;

	// try to find the attribute along the inherited path...
	if(getBaseType()!=nullptr){
		attr = getBaseType()->findTypeAttribute(id);
		if(attr!=nullptr)
			return attr;
	}
	// try to find the attribute from this type's type.
	return getType()!=nullptr ? getType()->findTypeAttribute(id) : nullptr;
}

//! ---|> Object
bool Type::setAttribute(const StringId & id,const Attribute & attr){
	attributes.setAttribute(id,attr);
	if(attr.isObjAttribute())
		setFlag(FLAG_CONTAINS_OBJ_ATTRS,true);
	return true;
}

void Type::copyObjAttributesTo(Object * instance){
	// init member vars of type
	if(getFlag(FLAG_CONTAINS_OBJ_ATTRS)){
		for(const auto & keyValuePair : attributes) {
			const Attribute & a = keyValuePair.second;
			if( a.isNull() || a.isTypeAttribute() )
				continue;
			instance->setAttribute(keyValuePair.first, Attribute(a.getValue()->getRefOrCopy(),a.getProperties()));
		}
	}
}

void Type::collectTypeAttributes(std::unordered_map<StringId,Object *> & attrs)const{
	for(const auto & keyValuePair : attributes) {
		if(keyValuePair.second.isTypeAttribute()) {
			attrs[keyValuePair.first] = keyValuePair.second.getValue();
		}
	}
}

void Type::collectObjAttributes(std::unordered_map<StringId,Object *> & attrs)const{
	for(const auto & keyValuePair : attributes) {
		if(keyValuePair.second.isObjAttribute()) {
			attrs[keyValuePair.first] = keyValuePair.second.getValue();
		}
	}
}

//! ---|> Object
void Type::collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs){
	for(const auto & keyValuePair : attributes) {
		attrs[keyValuePair.first] = keyValuePair.second.getValue();
	}
}

//! ---|> Object
bool Type::isA(Type * type) const {
	if(type == nullptr)
		return false;

	for(Type * t = getBaseType();t!=nullptr;t = t->getBaseType()){
		if(t==type)
			return true;
	}
	return Object::isA(type);
}

}
