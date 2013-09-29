// Object.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Object.h"
#include "../Basics.h"
#include "../StdObjects.h"
#include "../Consts.h"
#include "../Objects/Callables/Delegate.h"
#include "../Objects/Exception.h"
#include <sstream>

#ifdef ES_DEBUG_MEMORY
#include "../Utils/Debug.h"
#endif

namespace EScript{

//! (static)
Type * Object::getTypeObject(){
	static Type * typeObject = nullptr;
	if(typeObject==nullptr){
		// This object defines the basic type of all objects.
		// It does not inherit from anything else, but the type object itthisObj is of type 'Type'.
		typeObject = new Type(nullptr,nullptr);
		typeObject->typeRef = Type::getTypeObject();
	}
	return typeObject;
}

//! initMembers
void Object::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Object new Object
	ES_CTOR(typeObject,0,0,new Object(thisType))

	//! [ESMF] Object Object.clone()
	ES_FUN(typeObject,"clone",0,0,thisEObj->clone())

	//! [ESMF] Number Object.toNumber()
	ES_FUN(typeObject,"toNumber",0,0,thisEObj->toDouble())

	//! [ESMF] String Object.toString()
	ES_FUN(typeObject,"toString",0,0,thisEObj->toString())

	//! [ESMF] String Object.toDbgString()
	ES_FUN(typeObject,"toDbgString",0,0,thisEObj->toDbgString())

	//! [ESMF] Bool Object.isA(Object o)
	ES_FUN(typeObject,"isA",1,1,thisEObj->isA(parameter[0].toType<Type>()))

	//! [ESMF] Bool Object ---|> Object
	ES_FUN(typeObject,"---|>",1,1,thisEObj->isA(parameter[0].toType<Type>()))

	/*!	[ESMF] Bool Object.!=(Object o)
		\note Uses isEqual(...) which calls "=="-Operator
	*/
	ES_FUN(typeObject,"!=",1,1,!thisEObj->isEqual(rt,parameter[0]) )

	/*!	[ESMF] Bool Object.==(Object o)
		\note this is probably the only place where rt_isEqual(...) is called directly.	*/
	ES_FUN(typeObject,"==",1,1,thisEObj->rt_isEqual(rt,parameter[0]))

	//! [ESMF] Bool Object.===(Object o)
	ES_FUN(typeObject,"===",1,1,thisEObj->isIdentical(rt,parameter[0]))

	//! [ESMF] Bool Object.!==(Object o)
	ES_FUN(typeObject,"!==",1,1,!thisEObj->isIdentical(rt,parameter[0]))

	//! [ESMF] Bool !Object()
	ES_FUN(typeObject,"!_pre",0,0,!thisEObj->toBool())

	//! [ESMF] string Object.getTypeName()
	ES_FUN(typeObject,"getTypeName",0,0,thisEObj->getTypeName())

	//! [ESMF] Object Object.getType()
	ES_FUN(typeObject,"getType",0,0,thisEObj->getType())

	//! [ESMF] int Object.hash()
	ES_FUN(typeObject,"hash",0,0,thisEObj->hash().getValue())

	//! [ESMF] Object Object.getAttribute(key)
	ES_FUN(typeObject,"getAttribute",1,1,thisEObj->getAttribute(parameter[0].toString()).getValue())

	//! [ESMF] Object Object.getAttributeProperties(key)
	ES_FUN(typeObject,"getAttributeProperties",1,1,
				static_cast<unsigned int>(thisEObj->getAttribute(parameter[0].toString()).getProperties()))

	//! [ESMF] Object Object.getLocalAttribute(key)
	ES_FUN(typeObject,"getLocalAttribute",1,1,thisEObj->getLocalAttribute(parameter[0].toString()).getValue())

	//! [ESMF] Bool Object.isSet(key)
	ES_FUN(typeObject,"isSet",1,1,!thisEObj->getAttribute(parameter[0].toString()).isNull())

	//! [ESMF] Bool Object.isSetLocally(key)
	ES_FUN(typeObject,"isSetLocally",1,1,!thisEObj->getLocalAttribute(parameter[0].toString()).isNull())

	//! [ESMF] Bool Object.setAttribute(key,value(,flags = ATTR_NORMAL_ATTRIBUTE))
	ES_FUN(typeObject,"setAttribute",2,3,
				thisEObj->setAttribute(parameter[0].toString(),
													Attribute(parameter[1],
													static_cast<Attribute::flag_t>(parameter[2].to<int>(rt)))))

	//! [ESMF] Bool Object.assignAttribute(key,value)
	ES_FUN(typeObject,"assignAttribute",2,2,rt.assignToAttribute(thisEObj,parameter[0].toString(),parameter[1]))

	typedef std::unordered_map<StringId,Object *> attrMap_t; // has to be defined here, due to compiler (gcc) bug.
	
	//! Map Object._getAttributes()
	ES_FUNCTION(typeObject,"_getAttributes",0,0,{
		attrMap_t attrs;
		thisEObj->collectLocalAttributes(attrs);
		return Map::create(attrs);
	})

	//! Delegate Object -> function
	ES_FUN(typeObject,"->",1,1,Delegate::create(thisEObj,parameter[0]))

}


//! (static)
void ObjectReleaseHandler::release(Object * o) {
//	if(o->countReferences()!=0) {
//		std::cout << "\n !"<<o<<":"<<o->countReferences();
//		return;
//	}
	switch(o->_getInternalTypeId()){
		case _TypeIds::TYPE_NUMBER:{
			// the real c++ type can be somthing else than Number, but the typeId does not lie.
			if(o->getType()==Number::getTypeObject()) {
				Number * n = static_cast<Number *>(o);
				Number::release(n);
				return;
			}
			break;
		}
		case _TypeIds::TYPE_BOOL:{
			if(o->getType()==Bool::getTypeObject()) {
				Bool::release(static_cast<Bool*>(o));
				return;
			}
			break;
		}
		case _TypeIds::TYPE_STRING:{
			if(o->getType()==String::getTypeObject()) {
				String::release(static_cast<String*>(o));
				return;
			}
			break;
		}
		case _TypeIds::TYPE_DELEGATE:{
			Delegate::release(static_cast<Delegate*>(o));
			return;
		}
		case _TypeIds::TYPE_ARRAY:{
			if(o->getType()==Array::getTypeObject()) {
				Array::release(static_cast<Array*>(o));
				return;
			}
			break;
		}
		default:{}
	}
	delete o;
}

// ---------------------------------------------------

//! Constructor.
Object::Object():
		typeRef( getTypeObject() ){
#ifdef ES_DEBUG_MEMORY
	Debug::registerObj(this);
#endif
}

//! Constructor.
Object::Object(Type * _type):
		typeRef( _type ){
#ifdef ES_DEBUG_MEMORY
	Debug::registerObj(this);
#endif
}

//! Destructor.
Object::~Object() {
#ifdef ES_DEBUG_MEMORY
	Debug::unRegisterObj(this);
#endif
}
//! ---o
void Object::_assignValue(ObjPtr){
	throw new Exception("Cannot assign value.");
}

//! ---o
void Object::_initAttributes(Runtime &){}

//! ---o
Object * Object::clone() const {
	return new Object(getType());
}

//! ---o
bool Object::isA(Type * type) const {
	if(type == nullptr)
		return false;

	for(Type * t = getType();t!=nullptr;t = t->getBaseType()){
		if(t==type)
			return true;
	}
	return false;
}

//! ---o
std::string Object::toString()const {
	const Object * printableName = getAttribute(Consts::IDENTIFIER_attr_printableName).getValue();

	// #TYPENAME:0x42342
	// #PRINTABLENAME:TYPENAME:0x42342
	// \note If _printableName is neighter String nor Identifier it is ignored.
	//		This removes the possibility of endless recursions, e.g. if the _printableName is the Object itthisObj.
	std::ostringstream sprinter;
	sprinter << "#";
	if(printableName!=nullptr){
		const internalTypeId_t typeId = printableName->_getInternalTypeId();
		if(typeId==_TypeIds::TYPE_STRING ||typeId==_TypeIds::TYPE_IDENTIFIER)
			sprinter << printableName->toString() << ":";
	}

	sprinter << getTypeName()<<":"<<static_cast<const void*>(this);
	return sprinter.str();
}

//! ---o
int Object::toInt()const {
	return static_cast<int>(toDouble());
}

//! ---o
unsigned int Object::toUInt()const {
	return static_cast<unsigned int>(toDouble());
}

//! ---o
double Object::toDouble()const {
	return 0.0;//(double)std::atof(this->toString().c_str());
}

//! ---o
float Object::toFloat()const {
	return static_cast<float>(toDouble());
}

//! ---o
bool Object::toBool()const {
	return true;
}

//! ---o
std::string Object::toDbgString()const{
	return toString();
}

//! ---o
StringId Object::hash()const {
	return StringId(this->toString());
}

//! ---o
bool Object::rt_isEqual(Runtime &,const ObjPtr & other){
	return other == this ;
}

bool Object::isEqual(Runtime &runtime,const ObjPtr & other) {
	return callMemberFunction(runtime,this,Consts::IDENTIFIER_fn_equal,ParameterValues(other.get())).toBool();
}

Object * Object::getRefOrCopy() {
	return (!typeRef.isNull() && getType()->getFlag(Type::FLAG_CALL_BY_VALUE)) ? clone() : this;
}

bool Object::isIdentical(Runtime & rt,const ObjPtr & o) {
	if( (typeRef.isNotNull() && getType()->getFlag(Type::FLAG_CALL_BY_VALUE))){
		return o.isNotNull() && getType() == o->getType() && isEqual(rt,o);
	}else{
		return this==o.get();
	}
}



// -----------------------------------------------------------------------------------------------
// attributes

//! ---o
Attribute * Object::_accessAttribute(const StringId & id,bool localOnly){
	return (localOnly||getType()==nullptr) ? nullptr : getType()->findTypeAttribute(id);
}

const Attribute & Object::getLocalAttribute(const StringId & id)const{
	static const Attribute noAttribute;
	Object * nonConstThis = const_cast<Object*>(this);
	const Attribute * attr = nonConstThis->_accessAttribute(id,true);
	return attr == nullptr ? noAttribute : *attr;
}

const Attribute & Object::getAttribute(const StringId & id)const{
	static const Attribute noAttribute;
	Object * nonConstThis = const_cast<Object*>(this);
	const Attribute * attr = nonConstThis->_accessAttribute(id,false);
	return attr == nullptr ? noAttribute : *attr;
}

//! ---o
bool Object::setAttribute(const StringId & /*id*/,const Attribute & /*val*/){
	std::cout << "Could not set member.\n";
	return false;
}

// -----------------------------------------------------------------------------------------------
}
