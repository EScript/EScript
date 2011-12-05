// ExtObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_ExtObject_H
#define ES_ExtObject_H

#include "Object.h"
#include "../Utils/HashMap.h"

namespace EScript {

/*! [ExtObject] ---|> [Object]   */
class ExtObject : public Object {
		ES_PROVIDES_TYPE_NAME(ExtObject)

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -----

	/*! @name Main */
	//	@{
	public:
		static ExtObject * create();
		ExtObject();
		ExtObject(Type * parentExtObject);
		virtual ~ExtObject();

		/// ---|> [Object]
		virtual Object * clone() const;
	//	@}

	// -----

	/*! @name Attributes */
	//	@{
	public:
		typedef HashMap<ObjRef,identifierId> attributeMap_t;

		using Object::getAttribute;
		using Object::setObjAttribute;
		using Object::assignAttribute;

		/// ---|> [Object]
		virtual Object * getAttribute(const identifierId id);

		/// ---|> [Object]
		virtual bool setObjAttribute(const identifierId id,ObjPtr val);

		/// ---|> [Object]
		virtual bool assignAttribute(const identifierId id,ObjPtr val);

		/// ---|> [Object]
		virtual void getAttributes(std::map<identifierId,Object *> & attrs);

		void cloneAttributesFrom(const ExtObject * obj);

		inline attributeMap_t * getObjAttributes()const				{	return objAttributes;	}


		/*! Get an attribute that is directly stored in this Type-Object (NOT in an inherited Type) */
		Object * getObjAttribute(const identifierId id)const;
		inline Object * getObjAttribute(const char * key)const		{	return getObjAttribute(EScript::stringToIdentifierId(key));	}

		/*! Assign to an attribute that is directly stored in this Type-Object (NOT in an inherited Type) */
		bool assignObjAttribute(const identifierId id,ObjPtr val);

	private:
		attributeMap_t * objAttributes;
	// @}
};

}
#endif // ES_ExtObject_H
