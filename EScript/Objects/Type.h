// Type.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Type_H
#define ES_Type_H

#include "Object.h"
#include "../Utils/Attribute.h"
#include <stdint.h>

namespace EScript {

/*! [Type] ---|> [Object]   */
class Type : public Object {
		ES_PROVIDES_TYPE_NAME(Type)

	// -------------------------------------------------------------

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
	public:
		Type();
		Type(Type * baseType);
		Type(Type * baseType,Type * typeOfType);
		virtual ~Type();

		/// ---|> [Object]
		virtual Object * clone() const;

	//	@}

	// -------------------------------------------------------------

	/*! @name Attributes
		\note for an information about the using directives, see
			http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/topic/com.ibm.xlcpp8l.doc/language/ref/overload_member_fn_base_derived.htm
	*/
	// @{
	public:

		/*! Get only the typeAttributes. */
		void getTypeAttributes(std::map<identifierId,Object *> & attrs)const;
		/*! Get only the objectAttributes. */
		void getObjAttributes(std::map<identifierId,Object *> & attrs)const;

		void copyObjAttributesTo(Object * instance);

		/*! Used by instances of this type to assign a value to an inherited typeAttribute. */
		bool assignToTypeAttribute(const identifierId id,ObjPtr val);
		/*! Used by instances of this type get the value of an inherited typeAttribute. */
		Attribute * findTypeAttribute(const identifierId id);

		using Object::_accessAttribute;
		using Object::setAttribute;

		/// ---|> [Object]
		virtual Attribute * _accessAttribute(const identifierId id,bool localOnly);

		/// ---|> [Object]
		virtual bool setAttribute(const identifierId id,const Attribute & attr);

		/// ---|> [Object]
		virtual void getLocalAttributes(std::map<identifierId,Object *> & attrs);

	private:
		typedef std::map<identifierId,Attribute> AttributeMap_t;
		AttributeMap_t attributes;
	// @}

	// -------------------------------------------------------------

	/*! @name Flags */
	// @{
	public:
		typedef uint16_t flag_t;

		static const flag_t FLAG_CALL_BY_VALUE = 1<<0;
		static const flag_t FLAG_CONTAINS_OBJ_ATTRS = 1<<1;

		inline bool getFlag(flag_t f)const 			{	return (flags&f) >0;	}
		inline void setFlag(flag_t f,bool b=true) 	{	b? flags|=f : flags-=(flags&f);	}
		inline flag_t getFlags()const				{	return flags;	}
	private:
		flag_t flags;
	// @}

	// -------------------------------------------------------------

	/*! @name Inheritance */
	//	@{
	public:
		inline Type * getBaseType()const	{	return baseType.get();	}

		/// ---|> Object
		virtual bool isA(Type * type)const;

	private:
		ERef<Type> baseType;
	//	@}

};

}
#endif // ES_Type_H
