// Iterator.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
