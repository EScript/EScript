// YieldIterator.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_YIELD_ITERATOR_H
#define ES_YIELD_ITERATOR_H

#include "Type.h"
#include "../Runtime/FunctionCallContext.h"

namespace EScript {

//! [YieldIterator] ---|> [Object]
class YieldIterator : public Object {
		ES_PROVIDES_TYPE_NAME(YieldIterator)
	public:
		ESCRIPTAPI static Type* getTypeObject();
		ESCRIPTAPI static void init(EScript::Namespace & globals);
		// ----

		YieldIterator()	: Object(getTypeObject()),counter(0) {}
		virtual ~YieldIterator()					{	}

		Object * value()const					{	return currentValue.get();	}
		ESCRIPTAPI Object * key()const;
		void setValue(Object* newResult)		{	currentValue = newResult;	}

		_Ptr<FunctionCallContext> getFCC()const		{	return fcc;	}
		void setFCC(_Ptr<FunctionCallContext> _fcc)	{	fcc = _fcc;	}

		ESCRIPTAPI void next(Runtime & rt);
		bool end()const								{	return fcc.isNull();	}

		int getCounter()const						{	return counter;	}

	private:
		ObjRef currentValue;
		int counter;
		_CountedRef<FunctionCallContext> fcc;
};

}
#endif // ES_YIELD_ITERATOR_H
