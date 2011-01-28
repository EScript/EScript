#ifndef ES_Type_H
#define ES_Type_H

#include "../Object.h"
#include "Function.h"
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
		inline void setTypeAttribute(const char * key,ObjPtr val)		{	return setTypeAttribute(EScript::stringToIdentifierId(key),val);	}
		void setTypeAttribute(const identifierId id,ObjPtr val);

		/*! Get only the typeAttributes. */
		void getTypeAttributes(std::map<identifierId,Object *> & attrs);
		/*! Get only the objectAttributes. */
		void getObjAttributes(std::map<identifierId,Object *> & attrs);

		void initInstanceObjAttributes(Object * instance);

		/*! Get an attribute that is directly stored in this Type-Object (NOT in an inherited Type) */
		Object * getLocalAttribute(const identifierId id)const;
		inline Object * getLocalAttribute(const char * key)const		{	return getLocalAttribute(EScript::stringToIdentifierId(key));	}

		/*! Used by instances of this type to assign a value to an inherited typeAttribute. */
		bool assignToInheritedAttribute(const identifierId id,ObjPtr val);
		/*! Used by instances of this type get the value of an inherited typeAttribute. */
        Object * getInheritedAttribute(const identifierId id)const;

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

	private:
		/*! Internal representation of an object's attribute */
		class Attribute{
			public:
				enum attrType_t{ OBJECT, TYPE };
			private:
				ObjRef value;
				attrType_t attrType;
			public:
				Attribute(attrType_t _attrType=TYPE):attrType(_attrType) {}
				Attribute(Object * _value,attrType_t _attrType):value(_value),attrType(_attrType) {}
				Attribute(const Attribute & e):value(e.value),attrType(e.attrType) {}
				inline Object * getValue()const 	{	return value.get();	}
				inline bool isObjAttribute()const 	{	return attrType==OBJECT;	}
				inline bool isTypeAttribute()const 	{	return attrType==TYPE;	}
				inline void assign(Object * v)		{	value=v;	}
				inline Attribute & operator=(const Attribute & e){
					value=e.value, attrType=e.attrType;
					return *this;
				}
		};

		typedef std::map<identifierId,Attribute> AttributeMap_t;
		AttributeMap_t attr;
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
