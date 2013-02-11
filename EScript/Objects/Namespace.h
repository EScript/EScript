// Namespace.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_Namespace_H
#define ES_Namespace_H

#include "ExtObject.h"

namespace EScript {

//! [Namespace] ---|> [ExtObject] ---|> [Object]
class Namespace : public ExtObject {
		ES_PROVIDES_TYPE_NAME(Namespace)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);

		Namespace() : ExtObject()					{	}
		Namespace(Type * type) : ExtObject(type)	{	}
		virtual ~Namespace()						{	}

		//! ---|> [Object]
		virtual Namespace * clone() const;
};

}
#endif // ES_Namespace_H
