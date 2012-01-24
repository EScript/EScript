// ExtObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_ExtObject_H
#define ES_ExtObject_H

#include "Object.h"
#include "../Utils/Attribute.h"

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
		typedef std::map<identifierId,Attribute> attributeMap_t;

		using Object::_accessAttribute;
		using Object::setAttribute;
//		using Object::assignAttribute;

		/// ---|> [Object]
		virtual Attribute * _accessAttribute(const identifierId id,bool localOnly);

		/// ---|> [Object]
		virtual bool setAttribute(const identifierId id,const Attribute & attr);

//		/// ---|> [Object]
//		virtual bool assignAttribute(Runtime & rt,const identifierId id,ObjPtr val);

		/// ---|> [Object]
		virtual void getLocalAttributes(std::map<identifierId,Object *> & attrs);

		void cloneAttributesFrom(const ExtObject * obj);

		inline attributeMap_t * getObjAttributes()const				{	return objAttributes;	}


		/*! Get an attribute that is directly stored in this Type-Object (NOT in an inherited Type) */
		Object * getObjAttribute(const identifierId id)const;
		inline Object * getObjAttribute(const char * key)const		{	return getObjAttribute(EScript::stringToIdentifierId(key));	}

		/*! Assign to an attribute that is directly stored in this Type-Object (NOT in an inherited Type) */
		bool assignObjAttribute(Runtime & rt,const identifierId id,ObjPtr val);

	private:
		attributeMap_t * objAttributes;
	// @}
};

}
#endif // ES_ExtObject_H
