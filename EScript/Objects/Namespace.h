// Namespace.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_Namespace_H
#define ES_Namespace_H

#include "ExtObject.h"

namespace EScript {

//! [Namespace] ---|> [ExtObject] ---|> [Object]
class Namespace : public ExtObject {
		ES_PROVIDES_TYPE_NAME(Namespace)
	public:
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);

		Namespace() : ExtObject()					{	}
		Namespace(Type * type) : ExtObject(type)	{	}
		virtual ~Namespace()						{	}

		//! ---|> [Object]
		ESCRIPTAPI Namespace * clone() const override;
};

}
#endif // ES_Namespace_H
