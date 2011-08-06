// Identifier.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_IDENTIFIER_H_INCLUDED
#define ES_IDENTIFIER_H_INCLUDED

#include "Object.h"

namespace EScript{

/*! Wrapper object for an identifierId
	[Identifier]  ---|> [Object]  */
class Identifier:public Object{
	ES_PROVIDES_TYPE_NAME(Identifier)

	/*! @name Initialization */
	//	@{
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);
	//	@}

	// -----

		/*! (static) Factory */
		static Identifier * create( identifierId id);

		/*! (static) Factory */
		static Identifier * create( const std::string & s);

	private:
		Identifier(const identifierId &_id);
	public:
		virtual ~Identifier();

	private:
		identifierId id;
	public:
		identifierId getId()const {	return id;	}

		/// ---|> [Object]
		virtual Identifier * clone()const;
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr o);
		virtual std::string toString()const;
		virtual identifierId hash()const	{	return id;	}
};
//	@}


}
#endif // ES_IDENTIFIER_H_INCLUDED
