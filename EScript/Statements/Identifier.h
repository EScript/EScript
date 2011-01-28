// Identifier.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_IDENTIFIER_H_INCLUDED
#define ES_IDENTIFIER_H_INCLUDED

#include "../Object.h"

namespace EScript{

/*! Wrapper object for an identifierId
	[Identifier]  ---|> [Object]  */
class Identifier:public Object{
	public:
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
		virtual std::string toString()const;
		virtual identifierId hash()const	{	return id;	}
};
//	@}


}
#endif // ES_IDENTIFIER_H_INCLUDED
