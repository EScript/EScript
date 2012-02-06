// RuntimeContext.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_RUNTIMECONTEXT_H
#define ES_RUNTIMECONTEXT_H

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
		RuntimeBlock * createAndPushRTB(const Block * staticBlock);

		Object * getCaller()const    				{   return caller.get(); }
		RuntimeBlock * getCurrentRTB()const			{	return runtimeBlockStack.top().get();	}

		//! returns the line number of the previously executed statement or -1
		int getPrevLine()const						{	return (runtimeBlockStack.empty()||runtimeBlockStack.top()==NULL) ? -1 : runtimeBlockStack.top()->getPrevLine(); }

		//! \note assumes that the stack contains exactly one RTB, otherwise [this] is not set correctly
		void initCaller(const ObjPtr & obj);

		void popRTB()								{	runtimeBlockStack.pop();		}
		void pushRTB(RuntimeBlock * rtb)			{	runtimeBlockStack.push(rtb); }

	private:
		RuntimeContext();
		~RuntimeContext();
		void init();

		ObjRef caller;
		std::stack<RuntimeBlock::RTBRef> runtimeBlockStack;
};

}

#endif // ES_RUNTIMECONTEXT_H
