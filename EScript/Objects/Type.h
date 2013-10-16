// Type.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Type_H
#define ES_Type_H

#include "Object.h"
#include "../Utils/AttributeContainer.h"
#include <cstdint>

namespace EScript {

//! [Type] ---|> [Object]
class Type : public Object {
		ES_PROVIDES_TYPE_NAME(Type)

	// -------------------------------------------------------------

	//! @name Initialization
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -------------------------------------------------------------

	//! @name Main
	//	@{
	public:
		Type();
		Type(Type * baseType);
		Type(Type * baseType,Type * typeOfType);
		virtual ~Type();

		//! ---|> [Object]
		virtual Object * clone() const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_TYPE;	}
	//	@}

	// -------------------------------------------------------------

	/*! @name Attributes
		\note for an information about the using directives, see
			http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/topic/com.ibm.xlcpp8l.doc/language/ref/overload_member_fn_base_derived.htm
	*/
	// @{
	public:

		//! Get only the typeAttributes.
		void collectTypeAttributes(std::unordered_map<StringId,Object *> & attrs)const;
		//! Get only the objectAttributes.
		void collectObjAttributes(std::unordered_map<StringId,Object *> & attrs)const;

		void copyObjAttributesTo(Object * instance);

		//! Used by instances of this type get the value of an inherited typeAttribute.
		Attribute * findTypeAttribute(const StringId & id);

		using Object::_accessAttribute;
		using Object::setAttribute;

		//! ---|> [Object]
		virtual Attribute * _accessAttribute(const StringId & id,bool localOnly);

		//! ---|> [Object]
		virtual bool setAttribute(const StringId & id,const Attribute & attr);

		//! ---|> [Object]
		virtual void collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs);

	private:
		AttributeContainer attributes;
	// @}

	// -------------------------------------------------------------

	//! @name Flags
	// @{
	public:
		typedef uint16_t flag_t;

		static const flag_t FLAG_CALL_BY_VALUE = 1<<0;
		static const flag_t FLAG_CONTAINS_OBJ_ATTRS = 1<<1;
		static const flag_t FLAG_ALLOWS_USER_INHERITANCE = 1<<2;

		bool getFlag(flag_t f)const				{	return (flags&f) >0;	}
		void setFlag(flag_t f,bool b = true)	{	b? flags|=f : flags-=(flags&f);	}
		flag_t getFlags()const					{	return flags;	}
	private:
		flag_t flags;
	// @}

	// -------------------------------------------------------------

	//! @name Inheritance
	//	@{
	public:
		void allowUserInheritance(bool b)			{	setFlag(FLAG_ALLOWS_USER_INHERITANCE,b);	}
		bool allowsUserInheritance()const			{	return getFlag(FLAG_ALLOWS_USER_INHERITANCE);	}

		Type * getBaseType()const					{	return baseType.get();	}

		bool hasBase(Type * type)const;
		bool isBaseOf(Type * type)const;

	private:
		ERef<Type> baseType;
	//	@}

};

}
#endif // ES_Type_H
