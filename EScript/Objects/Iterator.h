// Iterator.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef E_ITERATOR_H
#define E_ITERATOR_H
#include "Type.h"

namespace EScript {

//! (abstract) [Iterator] ---|> [Object]
class Iterator : public Object {
		ES_PROVIDES_TYPE_NAME(Iterator)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);

		// ---
		Iterator(Type * type=nullptr) : Object(type?type:getTypeObject()) {}
		virtual ~Iterator()			{	}

		//! ---o
		virtual Object * key()		{	return nullptr;	}
		virtual Object * value()	{	return nullptr;	}
		virtual void reset()		{	}
		virtual void next()			{	}
		virtual bool end()			{	return true;	}
};
}

#endif // ITERATOR_H
