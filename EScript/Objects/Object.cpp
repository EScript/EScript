// Object.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Object.h"
#include "../EScript.h"
#include "../Consts.h"
#include <sstream>

#ifdef ES_DEBUG_MEMORY
#include "../Utils/Debug.h"
#endif

namespace EScript{

//! (static)
Type * Object::getTypeObject(){
	static Type * typeObject=NULL;
	if(typeObject==NULL){
		// This object defines the basic type of all objects.
		// It does not inherit from anything else, but the type object itself is of type 'Type'.
		typeObject=new Type(NULL,NULL);
		typeObject->typeRef=Type::getTypeObject();
	}
	return typeObject;
}

//! initMembers
void Object::init(EScript::Namespace & globals) {
	Type * typeObject=getTypeObject();
	typeObject->allowUserInheritance(true);
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Object new Object()
	ESMF_DECLARE(typeObject,Type,"_constructor",0,0,new Object(self))

	//! [ESMF] Object Object.clone()
	ESF_DECLARE(typeObject,"clone",0,0,caller->clone())

	//! [ESMF] Number Object.toNumber()
	ESF_DECLARE(typeObject,"toNumber",0,0,Number::create(caller->toDouble()))

	//! [ESMF] String Object.toString()
	ESF_DECLARE(typeObject,"toString",0,0,String::create(caller->toString()))

	//! [ESMF] String Object.toDbgString()
	ESF_DECLARE(typeObject,"toDbgString",0,0,String::create(caller->toDbgString()))

//	//! [ESMF] Object Object.execute() @deprecated
//	ESF_DECLARE(typeObject,"execute",0,0,runtime.executeObj(caller))

	//! [ESMF] Bool Object.isA(Object o)
	ESF_DECLARE(typeObject,"isA",1,1,Bool::create(caller->isA(parameter[0].toType<Type>())))

	//! [ESMF] Bool Object ---|> Object
	ESF_DECLARE(typeObject,"---|>",1,1,Bool::create(caller->isA(parameter[0].toType<Type>())))

	/*!	[ESMF] Bool Object.!=(Object o)
		\note Uses isEqual(...) which calls "=="-Operator
	*/
	ESF_DECLARE(typeObject,"!=",1,1,Bool::create(! caller->isEqual(runtime,parameter[0]) ))

	/*!	[ESMF] Bool Object.==(Object o)
		\note this is probably the only place where rt_isEqual(...) is called directly.	*/
	ESF_DECLARE(typeObject,"==",1,1,Bool::create(caller->rt_isEqual(runtime,parameter[0])))

	//! [ESMF] Bool Object.===(Object o)
	ESF_DECLARE(typeObject,"===",1,1,Bool::create(caller->isIdentical(runtime,parameter[0])))

	//! [ESMF] Bool Object.!==(Object o)
	ESF_DECLARE(typeObject,"!==",1,1,Bool::create(!caller->isIdentical(runtime,parameter[0])))

	//! [ESMF] Bool !Object()
	ESF_DECLARE(typeObject,"!_pre",0,0,Bool::create(!caller->toBool()))

	//! [ESMF] string Object.getTypeName()
	ESF_DECLARE(typeObject,"getTypeName",0,0,String::create(caller->getTypeName()))

	//! [ESMF] Object Object.getType()
	ESF_DECLARE(typeObject,"getType",0,0,caller->getType())

	//! [ESMF] int Object.hash()
	ESF_DECLARE(typeObject,"hash",0,0,Number::create(caller->hash().getValue()))

	//! [ESMF] Object Object.getAttribute(key)
	ESF_DECLARE(typeObject,"getAttribute",1,1,caller->getAttribute(parameter[0].toString()).getValue())

	//! [ESMF] Object Object.getAttributeProperties(key)
	ESF_DECLARE(typeObject,"getAttributeProperties",1,1,
				Number::create(static_cast<unsigned int>(caller->getAttribute(parameter[0].toString()).getProperties())))

	//! [ESMF] Bool Object.isSet(key)
	ESF_DECLARE(typeObject,"isSet",1,1,Bool::create(!caller->getAttribute(parameter[0].toString()).isNull()))

	//! [ESMF] Bool Object.setAttribute(key,value(,flags=ATTR_NORMAL_ATTRIBUTE))
	ESF_DECLARE(typeObject,"setAttribute",2,3,
				Bool::create(caller->setAttribute(parameter[0].toString(),
													Attribute(parameter[1],
													static_cast<Attribute::flag_t>(parameter[2].toInt())))))

	//! [ESMF] Bool Object.assignAttribute(key,value)
	ESF_DECLARE(typeObject,"assignAttribute",2,2,Bool::create(runtime.assignToAttribute(caller,parameter[0].toString(),parameter[1])))

	typedef std::unordered_map<StringId,Object *> attrMap_t; // has to be defined here, due to compiler (gcc) bug.
	//! Map Object._getAttributes()
	ES_FUNCTION_DECLARE(typeObject,"_getAttributes",0,0,{
		attrMap_t attrs;
		caller->collectLocalAttributes(attrs);
		return Map::create(attrs);
	})

	//! Delegate Object -> function
	ESF_DECLARE(typeObject,"->",1,1,new Delegate(caller,parameter[0]))

}


//! (static)
void ObjectReleaseHandler::release(Object * o) {
	if (o->countReferences()!=0) {
		std::cout << "\n !"<<o<<":"<<o->countReferences();
		return;
	}
	switch(o->_getInternalTypeId()){
		case _TypeIds::TYPE_NUMBER:{
			// the real c++ type can be somthing else than Number, but the typeId does not lie.
			if (o->getType()==Number::getTypeObject()) {
				Number * n=static_cast<Number *>(o);
				Number::release(n);
				return;
			}
			break;
		}
		case _TypeIds::TYPE_BOOL:{
			if (o->getType()==Bool::getTypeObject()) {
				Bool::release(static_cast<Bool*>(o));
				return;
			}
			break;
		}
		case _TypeIds::TYPE_STRING:{
			if (o->getType()==String::getTypeObject()) {
				String::release(static_cast<String*>(o));
				return;
			}
			break;
		}
		case _TypeIds::TYPE_ARRAY:{
			if (o->getType()==Array::getTypeObject()) {
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
	if (type == NULL)
		return false;

	for(Type * t=getType();t!=NULL;t=t->getBaseType()){
		if(t==type)
			return true;
	}
	return false;
}

//! ---o
std::string Object::toString()const {

	//! \todo fixme! Remove the const_cast! Why is getAttribute(...) not const?????
	const Object * printableName = const_cast<Object*>(this)->getAttribute(Consts::IDENTIFIER_attr_printableName).getValue();

	// #TYPENAME:0x42342
	// #PRINTABLENAME:TYPENAME:0x42342
	std::ostringstream sprinter;
	sprinter << "#";
	if(printableName!=NULL)
		sprinter << printableName->toString() << ":";

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
bool Object::rt_isEqual(Runtime &,const ObjPtr other){
	return other == this ;
}

bool Object::isEqual(Runtime &runtime,const ObjPtr other) {
	ObjRef resultRef=callMemberFunction(runtime,this,Consts::IDENTIFIER_fn_equal,ParameterValues(other));
	return resultRef.toBool();
}

Object * Object::getRefOrCopy() {
	return (!typeRef.isNull() && getType()->getFlag(Type::FLAG_CALL_BY_VALUE)) ? clone() : this;
}

bool Object::isIdentical(Runtime & rt,const ObjPtr o) {
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
	return (localOnly||getType()==NULL) ? NULL : getType()->findTypeAttribute(id);
}

const Attribute & Object::getLocalAttribute(const StringId & id)const{
	static const Attribute noAttribute;
	Object * nonConstThis = const_cast<Object*>(this);
	const Attribute * attr = nonConstThis->_accessAttribute(id,true);
	return attr == NULL ? noAttribute : *attr;
}

const Attribute & Object::getAttribute(const StringId & id)const{
	static const Attribute noAttribute;
	Object * nonConstThis = const_cast<Object*>(this);
	const Attribute * attr = nonConstThis->_accessAttribute(id,false);
	return attr == NULL ? noAttribute : *attr;
}

//! ---o
bool Object::setAttribute(const StringId & /*id*/,const Attribute & /*val*/){
	std::cout << "Could not set member.\n";
	return false;
}

// -----------------------------------------------------------------------------------------------
}
