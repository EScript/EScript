// Void.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef VOIDVALUE_H
#define VOIDVALUE_H

#include "../Type.h"

namespace EScript {

//! [Void] ---|> [Type] ---|> [Object]
class Void : public Type {
		ES_PROVIDES_TYPE_NAME(Void)
	public:
		static Void * get();

		//! ---|> [Object]
		virtual Object * clone()const;
		virtual bool toBool()const;
		virtual bool rt_isEqual(Runtime & rt,const ObjPtr & o);
		virtual std::string toString()const;
		virtual internalTypeId_t _getInternalTypeId()const	{	return _TypeIds::TYPE_VOID;	}

	protected:
		virtual ~Void();
		Void();
};
}

#endif // VOIDVALUE_H
