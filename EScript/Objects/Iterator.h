// Iterator.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef E_ITERATOR_H
#define E_ITERATOR_H
#include "Object.h"

namespace EScript {

/*! (abstract) [Iterator] ---|> [Object]    */
class Iterator : public Object {
		ES_PROVIDES_TYPE_NAME(Iterator)
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);

		// ---
		Iterator(Type * type=nullptr);
		virtual ~Iterator();

		/// ---o
		virtual Object * key();
		virtual Object * value();
		virtual void reset();
		virtual void next();
		virtual bool end();

		/// ---|> [Object]
};
}

#endif // ITERATOR_H
