// YieldIterator.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_YIELD_ITERATOR_H
#define ES_YIELD_ITERATOR_H

#include "Type.h"
#include "../Runtime/FunctionCallContext.h"

namespace EScript {

//! [YieldIterator] ---|> [Object]
class YieldIterator : public Object {
		ES_PROVIDES_TYPE_NAME(YieldIterator)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		// ----

		YieldIterator()	: Object(getTypeObject()),counter(0) {}
		virtual ~YieldIterator()					{	}

		Object * value()const					{	return currentValue.get();	}
		Object * key()const;
		void setValue(Object* newResult)		{	currentValue = newResult;	}

		_Ptr<FunctionCallContext> getFCC()const		{	return fcc;	}
		void setFCC(_Ptr<FunctionCallContext> _fcc)	{	fcc = _fcc;	}

		void next(Runtime & rt);
		bool end()const								{	return fcc.isNull();	}

		int getCounter()const						{	return counter;	}

	private:
		ObjRef currentValue;
		int counter;
		_CountedRef<FunctionCallContext> fcc;
};

}
#endif // ES_YIELD_ITERATOR_H
