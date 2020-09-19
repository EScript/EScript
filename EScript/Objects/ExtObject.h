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
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);
	//	@}

	// -----

	//! @name Main
	//	@{
	protected:
		ESCRIPTAPI ExtObject(const ExtObject & other);
	public:
		ESCRIPTAPI static ExtObject * create();
		ESCRIPTAPI ExtObject();
		ESCRIPTAPI ExtObject(Type * type);
		virtual ~ExtObject()	{ }

		//! ---|> [Object]
		ESCRIPTAPI Object * clone() const override;
	//	@}

	// -----

	//! @name Attributes
	//	@{
	public:

		using Object::_accessAttribute;
		using Object::setAttribute;

		//! ---|> [Object]
		ESCRIPTAPI Attribute * _accessAttribute(const StringId & id,bool localOnly) override;

		//! ---|> [Object]
		ESCRIPTAPI void _initAttributes(Runtime & rt) override;

		//! ---|> [Object]
		ESCRIPTAPI bool setAttribute(const StringId & id,const Attribute & attr) override;

		//! ---|> [Object]
		ESCRIPTAPI void collectLocalAttributes(std::unordered_map<StringId,Object *> & attrs) override;

		ESCRIPTAPI void cloneAttributesFrom(const ExtObject * obj);
	private:
		AttributeContainer objAttributes;
	// @}
};

}
#endif // ES_ExtObject_H
