// RuntimeBlock.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIMEBLOCK_H
#define RUNTIMEBLOCK_H

#include "../Objects/Internals/Block.h"
#include "../Utils/ObjRef.h"
#include "../Utils/LocalVarMap.h"
#include <stack>

namespace EScript {
class Object;
class RuntimeContext;

/*! [RuntimeBlock]    */
class RuntimeBlock:public EReferenceCounter<RuntimeBlock,RuntimeBlock> {
	private:
		static std::stack<RuntimeBlock *> pool;
	public:
		typedef _CountedRef<RuntimeBlock> RTBRef;

		static RuntimeBlock * create(RuntimeContext * _ctxt,const Block * staticBlock,RuntimeBlock * parentRTB=NULL);
		static void release(RuntimeBlock *rtb);

		// ----

		Block * getStaticBlock()const		{   return staticBlock; }

		bool assignToVariable(const identifierId id,Object * val);
		void initLocalVariable(const identifierId id,Object * val)		{	localVariables.declare(id,val);	}
		Object * getLocalVariable(const identifierId id)				{	return localVariables.find(id);	}

		void resetStatementCursor()	{	currentStatement=staticBlock->getStatements().begin();	}
		Statement * nextStatement()	{
			return currentStatement==staticBlock->getStatements().end() ? NULL : &(*currentStatement++);
		}
		void gotoStatement(int pos);

		//! returns the line number of the previously executed statement or -1
		int getPrevLine()const	{	return currentStatement==staticBlock->getStatements().begin() ? -1 : (currentStatement-1)->getLine(); }

	private:
		RuntimeBlock();
		~RuntimeBlock();
		void init(RuntimeContext * _ctxt,const Block * staticBlock,RuntimeBlock * parentRTB);

		Block * staticBlock;
		LocalVarMap localVariables;
		Block::statementCursor currentStatement;
		RuntimeContext * ctxt;
};

}

#endif // RUNTIMEBLOCK_H
