// Object.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef OBJECT_H
#define OBJECT_H
#include "../Utils/Attribute.h"
#include "../Utils/TypeNameMacro.h"
#include "../Utils/ObjRef.h"
#include "../Utils/Hashing.h"
#include "../Utils/EReferenceCounter.h"
#include "typeIds.h"

#include <iostream>
#include <map>

namespace EScript {

class Runtime;
class Namespace;
class Type;
class ObjectReleaseHandler;


/*! [Object]    */
class Object:public EReferenceCounter<Object,ObjectReleaseHandler>  {
		ES_PROVIDES_TYPE_NAME(Object)

	// -------------------------------------------------------------

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -------------------------

	/*! @name Main */
	//	@{
	public:
		//--

		//! Default constructor; Sets the type to Object::getTypeObject()
		Object();
		Object(Type * type);
		virtual ~Object();

		friend std::ostream & operator<<(std::ostream & s, EScript::Object * o) {
			return o==NULL ? s<<"NULL" :
					s<< "[" << o->getTypeName() << ":" <<static_cast<void*>(o)<< ":"<<o->countReferences()<< "]";
		}
		//! Return a clone of the object if the type is call-by-value and the object itself otherwise.
		Object * getRefOrCopy();

		//! ---o
		virtual Object * clone()const;
		//! ---o
		virtual identifierId hash()const;

		/*! ---o
			\note For camparing objects, never use this function directly but use isEqual(...) instead.
				  Otherwise scripted '=='-member functions are not supported. */
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr other);

		/*! Compare two Objects using the '=='-member function */
		bool isEqual(Runtime & rt,const ObjPtr o);

		/*! If this is an Object which is passed ...
			 -  call-by-value, this functions returns true if the given object's Type is the same as this' type
				and isEqual returns true.
			 - 	call-by-reference, this function returns true if the given object and this are the same Object. */
		bool isIdentical(Runtime & rt,const ObjPtr other);


		//! ---o
		//! For internal use only.
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_UNKNOWN; }
		
		
		//! ---o
		virtual void _assignValue(ObjPtr value);
	//	@}

	// -------------------------

	/*! @name Type */
	//	@{
	protected:
		ERef<Type> typeRef;
	public:
		inline Type * getType() const				{	return typeRef.get();	}

		/// ---o
		virtual bool isA(Type * type)const;
	//	@}

	// -------------------------

	/*! @name Attributes */
	public:
		
		//! ---o
		virtual Attribute * _accessAttribute(const identifierId id,bool localOnly);
		
		/*! Get the locally stored attribute with the given id.
			If the attribute is not found, the resulting attribute references NULL.
			\code 
				const Attribute & attr = obj->getLocalAttribute("attrName");
				if(attr.isNull()) ...
		*/
		const Attribute & getLocalAttribute(const identifierId id)const;
		const Attribute & getLocalAttribute(const char * key)const				{	return getLocalAttribute(EScript::stringToIdentifierId(key));	}
		
		/*! Get the attribute with the given id. The attribute can be stored locally or be accesible by the object's type.
			If the attribute is not found, the resulting attribute references NULL.
			\code 
				const Attribute & attr = obj->getAttribute("doesNotExist");
				if(attr.isNull()) ...
		*/
		const Attribute & getAttribute(const identifierId id)const;
		const Attribute & getAttribute(const char * key)const					{	return getAttribute(EScript::stringToIdentifierId(key));	}

		/*!	Try to set the value of an object attribute.
			Returns false if the attribute can not be set.
			---o */
		virtual bool setAttribute(const identifierId id,const Attribute & attr);
		inline  bool setAttribute(const char * key,const Attribute & attr)		{	return setAttribute(EScript::stringToIdentifierId(key),attr);	}

		/*! ---o
			Collect all attributes in a map; used for debugging. */
		virtual void getLocalAttributes(std::map<identifierId,Object *> & )	{	}
	// @}

	// -------------------------

	/*!	@name Conversion */
	//	@{
	public:
		//! ---o
		virtual std::string toString()const;
		//! ---o
		virtual int toInt()const;
		//! ---o
		virtual unsigned int toUInt()const;
		//! ---o
		virtual double toDouble()const;
		//! ---o
		virtual float toFloat()const;
		//! ---o
		virtual bool toBool()const;
		//! ---o
		virtual std::string toDbgString()const;
	//	@}

	// -------------------------
};

// ----------------------------------------------------------
class ObjectReleaseHandler{
	public:
		static void release(Object * obj);
};

struct ObjectPrinter : public std::unary_function<ObjRef, std::string> {
	std::string operator()(const ObjRef & ref) const {
		return ref.get()->toString();
	}
};

struct ObjectDebugPrinter : public std::unary_function<ObjRef, std::string> {
	std::string operator()(const ObjRef & ref) const {
		return ref.get()->toDbgString();
	}
};

}

#endif // OBJECT_H
