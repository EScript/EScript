// Identifier.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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
		virtual Identifier * clone()const		{	return create(id);	}
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr & o);
		virtual std::string toString()const		{	return id.toString();	}
		virtual StringId hash()const			{	return id;	}
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_IDENTIFIER;	}
};
//	@}


}
#endif // ES_IDENTIFIER_H_INCLUDED
