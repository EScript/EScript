// Void.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
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
		Object * clone()const override;
		bool toBool()const override;
		bool rt_isEqual(Runtime & rt,const ObjPtr & o) override;
		std::string toString()const override;
		internalTypeId_t _getInternalTypeId()const override	{	return _TypeIds::TYPE_VOID;	}

	protected:
		virtual ~Void();
		Void();
};
}

#endif // VOIDVALUE_H
