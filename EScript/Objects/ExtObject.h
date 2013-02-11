// ExtObject.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_ExtObject_H
#define ES_ExtObject_H

#include "Type.h"
#include "../Utils/AttributeContainer.h"

namespace EScript {

//! [ExtObject] ---|> [Object]
class ExtObject : public Object {
		ES_PROVIDES_TYPE_NAME(ExtObject)

	//! @name Initialization
	//	@{
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -----

	//! @name Main
	//	@{
	protected:
		ExtObject(const ExtObject & other);
	public:
		static ExtObject * create();
		ExtObject();
		ExtObject(Type * type);
		virtual ~ExtObject()	{ }

		//! ---|> [Object]
		virtual Object * clone() const;
	//	@}

	// -----

	//! @name Attributes
	//	@{
	public:

		using Object::_accessAttribute;
		using Object::setAttribute;

		//! ---|> [Object]
		virtual Attribute * _accessAttribute(const StringId & id,bool localOnly);

		//! ---|> [Object]
		virtual void _initAttributes(Runtime & rt);

		//! ---|> [Object]
		virtual bool setAttribute(const StringId & id,const Attribute & attr);

		//! ---|> [Object]
		virtual void collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs);

		void cloneAttributesFrom(const ExtObject * obj);
	private:
		AttributeContainer objAttributes;
	// @}
};

}
#endif // ES_ExtObject_H
