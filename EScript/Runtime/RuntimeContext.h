// RuntimeContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_RUNTIMESTATE_H
#define ES_RUNTIMESTATE_H

#include "RuntimeBlock.h"

#include <stack>

namespace EScript {

/*! [RuntimeContext]    */
class RuntimeContext:public EReferenceCounter<RuntimeContext,RuntimeContext> {
	private:
		static std::stack<RuntimeContext *> pool;
	public:
		typedef _CountedRef<RuntimeContext> RTBRef;

		static RuntimeContext * create();
		static void release(RuntimeContext *rts);

		// ----

		Object * getCaller()const    				{   return caller.get(); }

		//! \note assumes that the stack contains exactly one RTB, otherwise [this] is not set correctly
		void initCaller(const ObjPtr & obj);

		RuntimeBlock * createAndPushRTB(const Block * staticBlock);

		void pushRTB(RuntimeBlock * rtb)			{	runtimeBlockStack.push(rtb); }
		void popRTB()								{	runtimeBlockStack.pop();		}
		RuntimeBlock * getCurrentRTB()const			{	return runtimeBlockStack.top().get();	}

	private:
		RuntimeContext();
		~RuntimeContext();
		void init();

		ObjRef caller;
		std::stack<RuntimeBlock::RTBRef> runtimeBlockStack;
};

}

#endif // ES_RUNTIMESTATE_H
