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
	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Type new Type( [BaseType=ExtObject] )
	ESF_DECLARE(typeObject,"_constructor",0,1,parameter.count() == 0 ?
			new Type(ExtObject::getTypeObject()) :
			new Type(assertType<Type>(runtime,parameter[0])))


	//! [ESMF] Type Type.getBaseType()
	ESMF_DECLARE(typeObject,const Type,"getBaseType",0,0, self->getBaseType())


	// attrMap_t is declared outside of the getObjAttributes declaration as it otherwise leads to a strange
	// preprocessor error on gcc.
	typedef std::map<identifierId, Object *> attrMap_t;

	//! [ESMF] Map Type.getObjAttributes()
	ES_MFUNCTION_DECLARE(typeObject,const Type,"getObjAttributes",0,0,{
		attrMap_t attrs;
		self->getObjAttributes(attrs);
		return Map::create(attrs);
	})

	//! [ESMF] Map Type.getTypeAttributes()
	ES_MFUNCTION_DECLARE(typeObject,const Type,"getTypeAttributes",0,0,{
		attrMap_t attrs;
		self->getTypeAttributes(attrs);
		return Map::create(attrs);
	})

	//! [ESMF] self Object.setTypeAttribute(key,value)
	ESMF_DECLARE(typeObject,Type,"setTypeAttribute",2,2,
				(self->setTypeAttribute(parameter[0]->hash(),parameter[1]),self))
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
		getBaseType()->initInstanceObjAttributes(this);
	//ctor
}

//! (ctor)
Type::Type(Type * _baseType,Type * typeOfType):
		Object(typeOfType),flags(0),baseType(_baseType) {

	if(getBaseType()!=NULL)
		getBaseType()->initInstanceObjAttributes(this);
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
	"or adding object attributes to a Type whose instances can't store object attributes. ";

bool Type::assignToTypeAttribute(const identifierId id,ObjPtr val){
	Type * t=this;
	do{
		AttributeMap_t::iterator fIt=t->attr.find(id);
		if( fIt != t->attr.end()){
			if( fIt->second.isObjAttribute() ){
				std::string message = "(assignToTypeAttribute) type-attribute expected but object-attribute found. ('";
				message += identifierIdToString(id) + "')\n" + typeAttrErrorHint;
				throw new Exception(message);
			}
			(*fIt).second.assign(val.get());
			return true;
		}
		t=t->getBaseType();
	}while(t!=NULL);
	return false;
}

Object * Type::findTypeAttribute(const identifierId id)const{
	const Type * t=this;
	do{
		const AttributeMap_t::const_iterator fIt=t->attr.find(id);
		if( fIt != t->attr.end() ){
			if( fIt->second.isObjAttribute() ){
				std::string message = "(findTypeAttribute) type-attribute expected but object-attribute found. ('";
				message += identifierIdToString(id) + "')\n" + typeAttrErrorHint;
				throw new Exception(message);
			}
			return fIt->second.getValue();
		}
		t=t->getBaseType();
	}while(t!=NULL);
	return NULL;
}


//! ---|> Object
Object * Type::getAttribute(const identifierId id){
	// is local attribute?
	Object * result=this->getLocalAttribute(id);
	if(result!=NULL)
		return result;

	// try to find the attribute along the inheritated path...
	if(getBaseType()!=NULL){
		result = getBaseType()->findTypeAttribute(id);
		if(result!=NULL)
			return result;
	}

	// try to find the attribute from this type's type.
	return getType()!=NULL ? getType()->findTypeAttribute(id) : NULL;
}

Object * Type::getLocalAttribute(const identifierId id)const{
	const AttributeMap_t::const_iterator fIt=attr.find(id);
	return  fIt!=attr.end() ? fIt->second.getValue() : NULL ;
}

//! ---|> Object
bool Type::setObjAttribute(const identifierId id,ObjPtr val){
	attr[id] = Attribute(val.get(), Attribute::OBJECT);
	setFlag(FLAG_CONTAINS_OBJ_ATTRS,true);
	return true;
}

//! ---|> Object
bool Type::assignAttribute(const identifierId id,ObjPtr val){
	// try to assign to local attribute (object attribute or type attribute)
	AttributeMap_t::iterator fIt=attr.find(id);
	if( fIt != attr.end()){
		(*fIt).second.assign(val.get());
		return true;
	}

	// try to assign value along the inheritated path... (as type attribute only)
	if(getBaseType()!=NULL && getBaseType()->assignToTypeAttribute(id,val))
		return true;

	// try to assign the attribute to this type's type (if this is not the type of itself)
	return (getType()!=NULL  && getType()!=this) ? getType()->assignToTypeAttribute(id,val) : false;
}

void Type::setTypeAttribute(const identifierId id,ObjPtr val){
	attr[id] = Attribute(val.get(), Attribute::TYPE);
}

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

void Type::getTypeAttributes(std::map<identifierId,Object *> & attrs)const{
	for(AttributeMap_t::const_iterator it=attr.begin() ; it!=attr.end() ; ++it){
		if(it->second.isTypeAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

void Type::getObjAttributes(std::map<identifierId,Object *> & attrs)const{
	for(AttributeMap_t::const_iterator it=attr.begin() ; it!=attr.end() ; ++it){
		if(it->second.isObjAttribute())
			attrs[it->first] = it->second.getValue();
	}
}

//! ---|> Object
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

}
