// RuntimeBlock.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIMEBLOCK_H
#define RUNTIMEBLOCK_H

#include "../Object.h"
#include "../Statements/Block.h"
#include "../Utils/ObjRef.h"
#include "../Utils/LocalVarMap.h"
#include <stack>

namespace EScript {

/*! [RuntimeBlock]    */
class RuntimeBlock:public EReferenceCounter<RuntimeBlock,RuntimeBlock> {
	private:
		static std::stack<RuntimeBlock *> pool;
	public:
		typedef _CountedRef<RuntimeBlock> RTBRef;

		static RuntimeBlock * create(const Block * staticBlock,RuntimeBlock * _parentRTB=NULL,const ObjPtr & _caller=NULL);
		static void release(RuntimeBlock *rtb);

		// ----

		RuntimeBlock * getParent()const    	{   return parentRTB;  }
		Block * getStaticBlock()const		{   return staticBlock; }
		Object * getCaller()const    		{   return callerRef.get(); }
		void setCaller(Object * _caller);

		bool assignToVariable(const identifierId id,Object * val);
		void initLocalVariable(const identifierId id,Object * val)		{	localVariables.declare(id,val);	}
		Object * getLocalVariable(const identifierId id)				{	return localVariables.find(id);	}

	private:
		RuntimeBlock();
		~RuntimeBlock();
		void init(const Block * staticBlock,RuntimeBlock * parentRTB,const ObjPtr & _caller);

		Block * staticBlock;
		RuntimeBlock * parentRTB;
		ObjRef callerRef; // this
		LocalVarMap localVariables;
};

}

#endif // RUNTIMEBLOCK_H
