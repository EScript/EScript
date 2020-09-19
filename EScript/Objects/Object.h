// Object.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef OBJECT_H
#define OBJECT_H
#include "../Utils/Attribute.h"
#include "../Utils/TypeNameMacro.h"
#include "../Utils/ObjRef.h"
#include "../Utils/Hashing.h"
#include "../Utils/EReferenceCounter.h"
#include "typeIds.h"

#include <ostream>
#include <unordered_map>

namespace EScript {

class Runtime;
class Namespace;
class Type;
class ObjectReleaseHandler;

//! [Object]
class Object:public EReferenceCounter<Object,ObjectReleaseHandler>  {
		ES_PROVIDES_TYPE_NAME(Object)

	// -------------------------------------------------------------

	//! @name Initialization
	//	@{
	public:
		ESCRIPTAPI static Type * getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);
	//	@}

	// -------------------------

	//! @name Main
	//	@{
	public:
		//--

		//! Default constructor; Sets the type to Object::getTypeObject()
		ESCRIPTAPI Object();
		ESCRIPTAPI Object(Type * type);
		ESCRIPTAPI virtual ~Object();

		friend std::ostream & operator<<(std::ostream & s, EScript::Object * o) {
			return o==nullptr ? s<<"nullptr" :
					s<< "[" << o->getTypeName() << ":" <<static_cast<void*>(o)<< ":"<<o->countReferences()<< "]";
		}
		//! Return a clone of the object if the type is call-by-value and the object itthisObj otherwise.
		ESCRIPTAPI Object * getRefOrCopy();

		//! ---o
		ESCRIPTAPI virtual Object * clone()const;
		//! ---o
		ESCRIPTAPI virtual StringId hash()const;

		/*! ---o
			\note For camparing objects, never use this function directly but use isEqual(...) instead.
				  Otherwise scripted '=='-member functions are not supported. */
		ESCRIPTAPI virtual bool rt_isEqual(Runtime & rt,const ObjPtr & other);

		//! Compare two Objects using the '=='-member function
		ESCRIPTAPI bool isEqual(Runtime & rt,const ObjPtr & o);

		/*! If this is an Object which is passed ...
			 -  call-by-value, this functions returns true if the given object's Type is the same as this' type
				and isEqual returns true.
			 - 	call-by-reference, this function returns true if the given object and this are the same Object. */
		ESCRIPTAPI bool isIdentical(Runtime & rt,const ObjPtr & other);


		//! ---o
		//! For internal use only.
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_UNKNOWN; }


		/*! If this object is the value of an attribute marked as reference, and a new value is assigned to this attribute,
			this function is called instead of setting the new object as value for the attribute.
			This makes only sense for reference objects like NumberRef.
			\todo Unused! Check if still useful.
			---o	*/
		ESCRIPTAPI virtual void _assignValue(ObjPtr value);

	//	@}

	// -------------------------

	//! @name Type
	//	@{
	protected:
		ERef<Type> typeRef;
	public:
		inline Type * getType() const				{	return typeRef.get();	}

		//! ---o
		ESCRIPTAPI virtual bool isA(const Type * type)const;
	//	@}

	// -------------------------

	//! @name Attributes
	public:
		/*! ---o (internal)
			Get access to an Attribute stored at this Object.
			\note Should not be called directly. Use get(Local)Attribute(...) instead.
			\note Has to be overridden if an Object type should support user defined attributes.
		*/
		ESCRIPTAPI virtual Attribute * _accessAttribute(const StringId & id,bool localOnly);

		/*! ---o (internal)
		This function is called by the runtime after a new Object has been created in the script using "new". The
			execution takes place after the Object itthisObj has been created, but before the first scripted constructor is executed.
			Extended attribute initializations can be performed here.
			\note Has to be overridden if an Object type should support user defined attributes. */
		ESCRIPTAPI virtual void _initAttributes(Runtime & rt);

		/*! Get the locally stored attribute with the given id.
			If the attribute is not found, the resulting attribute references nullptr.
			\code
				const Attribute & attr = obj->getLocalAttribute("attrName");
				if(attr.isNull()) ...
		*/
		ESCRIPTAPI const Attribute & getLocalAttribute(const StringId & id)const;

		/*! Get the attribute with the given id. The attribute can be stored locally or be accessible by the object's type.
			If the attribute is not found, the resulting attribute references nullptr.
			\code
				const Attribute & attr = obj->getAttribute("doesNotExist");
				if(attr.isNull()) ...
		*/
		ESCRIPTAPI const Attribute & getAttribute(const StringId & id)const;
		const Attribute & getAttribute(const char * c_str)const					{	return getAttribute(StringId(c_str));	}

		/*!	---o
			Try to set the value of an object attribute.
			Returns false if the attribute can not be set.
			\note Has to be overridden if an Object type should support user defined attributes. */
		ESCRIPTAPI virtual bool setAttribute(const StringId & id,const Attribute & attr);
		bool setAttribute(const char * c_str,const Attribute & attr)			{	return setAttribute(StringId(c_str),attr);	}

		/*! ---o
			Collect all attributes in a map; used for debugging. */
		virtual void collectLocalAttributes(std::unordered_map<StringId,Object *> & )		{	}
	// @}

	// -------------------------

	//! @name Conversion
	//	@{
	public:
		//! ---o
		ESCRIPTAPI virtual std::string toString()const;
		//! ---o
		ESCRIPTAPI virtual int toInt()const;
		//! ---o
		ESCRIPTAPI virtual unsigned int toUInt()const;
		//! ---o
		ESCRIPTAPI virtual double toDouble()const;
		//! ---o
		ESCRIPTAPI virtual float toFloat()const;
		//! ---o
		ESCRIPTAPI virtual bool toBool()const;
		//! ---o
		ESCRIPTAPI virtual std::string toDbgString()const;
	//	@}

	// -------------------------
};

// ----------------------------------------------------------
class ObjectReleaseHandler{
	public:
		ESCRIPTAPI static void release(Object * obj);
};

}

#endif // OBJECT_H
