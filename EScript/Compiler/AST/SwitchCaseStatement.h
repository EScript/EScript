// SwitchCaseStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_SWITCH_CASE_STATEMENT_H
#define ES_SWITCH_CASE_STATEMENT_H

#include "ASTNode.h"
#include "Block.h"

namespace EScript {
namespace AST {

//! [SwitchCaseStatement]  ---|> [ASTNode]
class SwitchCaseStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(SwitchCaseStatement)
	public:
		//! [ (statementIndex,conditionExpr)* ]
		typedef std::vector<std::pair<size_t,ERef<AST::ASTNode>>> caseInfoVector_t;
		SwitchCaseStatement( ptr_t _decisionExpr,
							Block * _block,caseInfoVector_t && _caseInfos) :
			ASTNode(TYPE_SWITCH_STATEMENT,false),
			block(_block),
			decisionExpr(_decisionExpr),
			caseInfos(_caseInfos){}
		virtual ~SwitchCaseStatement() {}

		ptr_t getDecisionExpression()const				{	return decisionExpr;	}
		Block* getBlock()const							{	return block.get();	}
		const caseInfoVector_t & getCaseInfos()const	{	return caseInfos;	}
	private:
		ERef<Block> block;
		ref_t decisionExpr;
		caseInfoVector_t caseInfos;
};
}
}

#endif // ES_SWITCH_CASE_STATEMENT_H
