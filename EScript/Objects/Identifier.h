// Identifier.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_IDENTIFIER_H_INCLUDED
#define ES_IDENTIFIER_H_INCLUDED

#include "Type.h"

namespace EScript{

/*! Wrapper object for an StringId
	[Identifier]  ---|> [Object]  */
class Identifier:public Object{
		ES_PROVIDES_TYPE_NAME(Identifier)

	//! @name Initialization
	//	@{
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -----

		//! (static) Factory
		static Identifier * create( StringId id);

		//! (static) Factory
		static Identifier * create( const std::string & s);

	private:
		Identifier(const StringId &_id);
	public:
		virtual ~Identifier()					{ }

	private:
		StringId id;
	public:
		StringId & operator*()					{	return id;	}
		StringId operator*()const				{	return id;	}

		StringId getId()const					{	return id;	}

		//! ---|> [Object]
		Identifier * clone()const override		{	return create(id);	}
		bool rt_isEqual(Runtime & rt,const ObjPtr & o) override;
		std::string toString()const override		{	return id.toString();	}
		StringId hash()const override			{	return id;	}
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_IDENTIFIER;	}
};
//	@}


}
#endif // ES_IDENTIFIER_H_INCLUDED
