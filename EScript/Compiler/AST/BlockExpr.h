// BlockExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_BLOCK_EXPR_H
#define ES_BLOCK_EXPR_H

#include "../Object.h"
#include "Statement.h"

#include <deque>
#include <set>

namespace EScript {
namespace AST {

//! [BlockExpr]  ---|> [Object]
class BlockExpr : public Object {
		ES_PROVIDES_TYPE_NAME(BlockExpr)
	public:
		typedef std::deque<Statement > statementList;
		typedef statementList::iterator statementCursor;
		typedef statementList::const_iterator cStatementCursor;
		typedef std::set<StringId>  declaredVariableMap_t;

		BlockExpr(int lineNr=-1) : line(lineNr) {}
		virtual ~BlockExpr() {}

		statementList & getStatements()					{	return statements;	}
		const statementList & getStatements()const		{	return statements;	}
		int getLine()const								{	return line;	}


		//! returns false if variable was already declared
		bool declareVar(StringId id)					{	return vars.insert(id).second;	}
		const declaredVariableMap_t & getVars()const	{	return vars;	}
		bool isLocalVar(StringId id)					{	return vars.count(id)>0;	}
		void addStatement(const Statement & s)			{	if(s.isValid())		statements.push_back(s);	}
		bool hasLocalVars()const						{	return !vars.empty(); }
		size_t getNumLocalVars()const					{	return vars.size(); }

		//! ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_BLOCK_STATEMENT; }
	private:
		declaredVariableMap_t vars;
		statementList statements;
		int line;
};
}
}

#endif // ES_BLOCK_EXPR_H
