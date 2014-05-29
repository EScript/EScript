// ExtObject.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
		Object * clone() const override;
	//	@}

	// -----

	//! @name Attributes
	//	@{
	public:

		using Object::_accessAttribute;
		using Object::setAttribute;

		//! ---|> [Object]
		Attribute * _accessAttribute(const StringId & id,bool localOnly) override;

		//! ---|> [Object]
		void _initAttributes(Runtime & rt) override;

		//! ---|> [Object]
		bool setAttribute(const StringId & id,const Attribute & attr) override;

		//! ---|> [Object]
		void collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs) override;

		void cloneAttributesFrom(const ExtObject * obj);
	private:
		AttributeContainer objAttributes;
	// @}
};

}
#endif // ES_ExtObject_H
