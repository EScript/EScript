// RuntimeBlock.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RUNTIMEBLOCK_H
#define RUNTIMEBLOCK_H

#include "../Objects/AST/BlockStatement.h"
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

		static RuntimeBlock * create(RuntimeContext * _ctxt,const AST::BlockStatement * staticBlock,RuntimeBlock * parentRTB=NULL);
		static void release(RuntimeBlock *rtb);

		// ----

		AST::BlockStatement * getStaticBlock()const		{   return staticBlock; }

		bool assignToVariable(Runtime & rt,const StringId id,Object * val);
		void initLocalVariable(const StringId id,Object * val)		{	localVariables.declare(id,val);	}
		Object * getLocalVariable(const StringId id)				{	return localVariables.find(id);	}

		void resetStatementCursor()	{	currentStatement=staticBlock->getStatements().begin();	}
		AST::Statement * nextStatement()	{
			return currentStatement==staticBlock->getStatements().end() ? NULL : &(*currentStatement++);
		}
		void gotoStatement(int pos);

		//! returns the line number of the previously executed statement or -1
		int getPrevLine()const	{	return currentStatement==staticBlock->getStatements().begin() ? -1 : (currentStatement-1)->getLine(); }

	private:
		RuntimeBlock();
		~RuntimeBlock();
		void init(RuntimeContext * _ctxt,const AST::BlockStatement * staticBlock,RuntimeBlock * parentRTB);

		AST::BlockStatement * staticBlock;
		LocalVarMap localVariables;
		AST::BlockStatement::statementCursor currentStatement;
		RuntimeContext * ctxt;
};

}

#endif // RUNTIMEBLOCK_H
