#include "Object.h"
#include "EScript.h"
#include <sstream>

#ifdef ES_DEBUG_MEMORY
#include "Utils/Debug.h"
#endif

namespace EScript{

/*! (static) */
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

namespace EMembers{

/*! \name EClass: Object
    \see EScript::Object
*/
//@{

/*!	[ESMF] Object new Object()	*/
ESF(esmf_Object_constructor,0,0,new Object(dynamic_cast<Type*>(caller)))

/*!	[ESMF] Object Object.clone()	*/
ESF(esmf_Object_clone,0,0,caller->clone())

/*!	[ESMF] Number Object.toNumber()	*/
ESF(esmf_Object_toNumber,0,0,Number::create(caller->toDouble()))

/*!	[ESMF] String Object.toString()	*/
ESF(esmf_Object_toString,0,0,String::create(caller->toString()))

/*!	[ESMF] String Object.toDbgString()	*/
ESF(esmf_Object_toDbgString,0,0,String::create(caller->toDbgString()))

/*!	[ESMF] Object Object.execute()	*/
ES_FUNCTION(esmf_Object_execute) {
	assertParamCount(runtime,parameter.count(),0,0);
	return runtime.executeObj(caller);
}

/*!	[ESMF] Bool Object.extends(Object o)	*/
ESF(esmf_Object_extends,1,1,Bool::create(caller->isA(parameter[0].toType<Type>())))

/*!	[ESMF] Bool Object.!=(Object o)
	\note Uses isEqual(...) which calls "=="-Operator
*/
ESF(esmf_Object_notEqual,1,1,Bool::create(! caller->isEqual(runtime,parameter[0]) ))

/*!	[ESMF] Bool Object.==(Object o)
	\note this is probably the only place where rt_isEqual(...) is called directly.	*/
ESF(esmf_Object_equal,1,1,Bool::create(caller->rt_isEqual(runtime,parameter[0])))

/*!	[ESMF] Bool Object.===(Object o)	*/
ESF(esmf_Object_identical,1,1,Bool::create(caller->isIdentical(runtime,parameter[0])))

/*!	[ESMF] Bool Object.!==(Object o)	*/
ESF(esmf_Object_notIdentical,1,1,Bool::create(!caller->isIdentical(runtime,parameter[0])))

/*!	[ESMF] Bool !Object()	*/
ESF(esmf_Object_neg,0,0,Bool::create(!caller->toBool()))

/*!	[ESMF] string Object.getTypeName()	*/
ESF(esmf_Object_getTypeName,0,0,String::create(caller->getTypeName()))

/*!	[ESMF] Object Object.getType()	*/
ESF(esmf_Object_getType,0,0,caller->getType())

/*!	[ESMF] int Object.hash()	*/
ESF(esmf_Object_hash,0,0,Number::create(caller->hash()))

/*!	[ESMF] Object Object.getAttribute(key)	*/
ESF(esmf_Object_getAttribute,1,1,caller->getAttribute(parameter[0]->hash()))

/*!	[ESMF] Bool Object.isSet(key)	*/
ESF(esmf_Object_isSet,1,1,Bool::create(caller->getAttribute(parameter[0]->hash())))

/*!	[ESMF] Bool Object.setObjAttribute(key,value)	*/
ESF(esmf_Object_setObjAttribute,2,2,Bool::create(caller->setObjAttribute(parameter[0]->hash(),parameter[1])))

/*!	[ESMF] Bool Object.assignAttribute(key,value)	*/
ESF(esmf_Object_assignAttribute,2,2,Bool::create(caller->assignAttribute(parameter[0]->hash(),parameter[1])))

/*!	Map Object._getAttributes()	*/
ES_FUNCTION(esmf_Object_getAttributes){
	assertParamCount(runtime,parameter.count(),0,0);
	std::map<identifierId,Object *>  attrs;
	caller->getAttributes(attrs);
	return Map::create(attrs);
}

/*!	Delegate Object -> function	*/
ESF(esmf_Object_createDelegate,1,1,new Delegate(caller,parameter[0]))

/*!	string Object.tmp_toByteCode()	*/
ES_FUNCTION(esmf_Object_tmp_toByteCode){
	std::ostringstream s;
	caller->tmp_toByteCode(s);
	return String::create(s.str());
}
//@}
} // namespace EMembers

/*!	initMembers	*/
void Object::init(EScript::Namespace & globals) {

	using namespace EMembers;
	Type * c=getTypeObject();
	declareConstant(&globals,getClassName(),c);

	declareFunction(c, "clone" , 			*esmf_Object_clone);
	declareFunction(c, "_constructor",		*esmf_Object_constructor);
	declareFunction(c, "---|>",				*esmf_Object_extends);
	declareFunction(c, "isA",				*esmf_Object_extends);
	declareFunction(c, "toNumber",			*esmf_Object_toNumber);
	declareFunction(c, "toString",			*esmf_Object_toString);
	declareFunction(c, "toDbgString",		*esmf_Object_toDbgString);
	declareFunction(c, "execute",			*esmf_Object_execute);
	declareFunction(c, "!=",				*esmf_Object_notEqual);
	declareFunction(c, "==",				*esmf_Object_equal);
	declareFunction(c, "===",				*esmf_Object_identical);
	declareFunction(c, "!==",				*esmf_Object_notIdentical);
	declareFunction(c, "!_pre",				*esmf_Object_neg);

//	declareFunction(c, "getBaseType",		*esmf_Object_getBaseType);
	declareFunction(c, "getType",			*esmf_Object_getType);
	declareFunction(c, "getTypeName",		*esmf_Object_getTypeName);
	declareFunction(c, "hash",				*esmf_Object_hash);
	declareFunction(c, "getAttribute",		*esmf_Object_getAttribute);
	declareFunction(c, "isSet",				*esmf_Object_isSet); // search only locally ???
	//declareFunction( "getAttributes",	*esmf_Object_getAttributes);
//	declareFunction(c, "setAttribute",		*esmf_Object_setTypeAttribute); // deprecated
////	declareFunction(c, "setTypeAttribute",*esmf_Object_setTypeAttribute);
	declareFunction(c, "setObjAttribute",	*esmf_Object_setObjAttribute);
	declareFunction(c, "assignAttribute",	*esmf_Object_assignAttribute);


	declareFunction(c,"_getAttributes",		*esmf_Object_getAttributes);
//	declareFunction(c,"_getFlags",			*esmf_Object_getFlags);

    declareFunction(c,"->",					*esmf_Object_createDelegate);

    declareFunction(c,"tmp_toByteCode",		*esmf_Object_tmp_toByteCode); // experimental

}

/*!	(static)	*/
void ObjectReleaseHandler::release(Object * o) {
    if (o->countReferences()!=0) {
        std::cout << "\n !"<<o<<":"<<o->countReferences();
        return;
    }
	if (o->getType()==Number::typeObject) {
		Number * n=dynamic_cast<Number *>(o);
		if(n!=NULL){ // n can be null if o is a Type that inherits from Number but is not a Type
			Number::release(n);
			return;
		}
	}else if (o->getType()==Bool::typeObject) {
		Bool * b=dynamic_cast<Bool *>(o);
		if(b!=NULL){
			Bool::release(b);
			return;
		}
	}else if (o->getType()==String::typeObject) {
		String * s=dynamic_cast<String *>(o);
		if(s!=NULL){
			String::release(s);
			return;
		}
	}else if (o->getType()==Array::typeObject) {
		Array * a=dynamic_cast<Array *>(o);
		if(a!=NULL){
			Array::release(a);
			return;
		}
	}
	delete o;
}

// ---------------------------------------------------

/*!	Constructor.	*/
Object::Object():
		typeRef( getTypeObject() ){
#ifdef ES_DEBUG_MEMORY
	Debug::registerObj(this);
#endif
}

/*!	Constructor.	*/
Object::Object(Type * _type):
		typeRef( _type ){
#ifdef ES_DEBUG_MEMORY
	Debug::registerObj(this);
#endif
}

/*!	Destructor.	*/
Object::~Object() {
#ifdef ES_DEBUG_MEMORY
	Debug::unRegisterObj(this);
#endif
}

/*!	---o	*/
Object * Object::clone() const {
	return new Object(getType());
}

/*!	---o */
bool Object::isA(Type * type) const {
	if (type == NULL)
		return false;

	for(Type * t=getType();t!=NULL;t=t->getBaseType()){
		if(t==type)
			return true;
	}
	return false;
}

/*!	---o	*/
std::string Object::toString()const {
	return std::string("#")+getTypeName();
}

/*!	---o	*/
int Object::toInt()const {
	return static_cast<int>(toDouble());
}

/*!	---o	*/
double Object::toDouble()const {
	return 0.0;//(double)std::atof(this->toString().c_str());
}

/*!	---o	*/
float Object::toFloat()const {
	return toDouble();
}

/*!	---o	*/
bool Object::toBool()const {
	return true;
}

//! ---o
std::string Object::toDbgString()const{
	return toString();
}

/*!	---o	*/
identifierId Object::hash()const {
	return EScript::stringToIdentifierId(this->toString());
}

/*!	---o	*/
Object * Object::execute(Runtime & /*rt*/) {
	return getRefOrCopy();
}

/*!	---o	*/
bool Object::rt_isEqual(Runtime &,const ObjPtr other){
	return other == this ;
}

/*!	*/
bool Object::isEqual(Runtime &runtime,const ObjPtr other) {
    ObjRef resultRef=callMemberFunction(runtime,this,Consts::IDENTIFIER_fn_equal,ParameterValues(other));
    return resultRef.toBool();
}

/*!	*/
Object * Object::getRefOrCopy() {
	return (!typeRef.isNull() && getType()->getFlag(Type::FLAG_CALL_BY_VALUE)) ? clone() : this;
}


/*!	*/
bool Object::isIdentical(Runtime & rt,const ObjPtr o) {
	if( (typeRef.notNull() && getType()->getFlag(Type::FLAG_CALL_BY_VALUE))){
		return o.notNull() && getType() == o->getType() && isEqual(rt,o);
	}else{
		return this==o.get();
	}
}



// -----------------------------------------------------------------------------------------------
// attributes

/*!	---o */
Object * Object::getAttribute(const identifierId id){
	return getType()!=NULL ? getType()->getInheritedAttribute(id) : NULL ;
}

/*!	---o */
bool Object::setObjAttribute(const identifierId /*id*/,ObjPtr /*val*/){
	std::cout << "Could not set member.\n";
	return false;
}

/*!	---o */
bool Object::assignAttribute(const identifierId id,ObjPtr val){
	return getType()==NULL ? false : getType()->assignToInheritedAttribute(id,val);
}

// -----------------------------------------------------------------------------------------------

/*!		*/
void Object::tmp_toByteCode(ostream &s){
	s<<"\"Obj:"<<toString()<<"\"";
}
}
