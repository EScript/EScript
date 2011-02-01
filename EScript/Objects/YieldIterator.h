// YieldIterator.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_YIELD_ITERATOR_H
#define ES_YIELD_ITERATOR_H

#include "Object.h"
#include "../Runtime/RuntimeContext.h"
#include <stack>

namespace EScript {

/*! [YieldIterator] ---|> [Object]   */
class YieldIterator : public Object {
		ES_PROVIDES_TYPE_NAME(YieldIterator)
	public:
		static Type* typeObject;
		static void init(EScript::Namespace & globals);
		// ----

		YieldIterator();
		virtual ~YieldIterator();

		Object * value()const						{	return myResult.get();	}
		Object * key()const	;
		void setResult(ObjPtr newResult)			{	myResult=newResult;		}

		void setContext(RuntimeContext * ctxt)		{	runtimeContext=ctxt;	}
		RuntimeContext * getContext()const			{	return runtimeContext.get();	}

		void next(Runtime & rt);
		bool end()const								{	return !active;	}

		int getCounter()const						{	return counter;	}

		/// ---|> [Object]
//		virtual YieldIterator * clone() const;
//		virtual bool rt_isEqual(Runtime &rt, const ObjPtr o);

	private:
		ObjRef myResult;
		RuntimeContext::RTBRef  runtimeContext;
		int counter;
		bool active;
};

}
#endif // ES_YIELD_ITERATOR_H
