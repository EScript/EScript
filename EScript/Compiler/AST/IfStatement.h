// IfStatement.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_IF_STAMTEN_H
#define ES_IF_STAMTEN_H

#include "ASTNode.h"

namespace EScript {
namespace AST {

//! [IfStatement]  ---|> [ASTNode]
class IfStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(IfStatement)
	public:
		IfStatement( ptr_t _condition, ptr_t _action,ptr_t _elseAction):
			ASTNode(TYPE_IF_STATEMENT,false),condition(_condition),actionStatement(_action),elseActionStatement(_elseAction) {}
		virtual ~IfStatement() {}

		ptr_t getCondition()const		{	return condition;	}
		ptr_t getAction()const			{	return actionStatement;	}
		ptr_t getElseAction()const		{	return elseActionStatement;	}

	private:
		ref_t condition;
		ref_t actionStatement;
		ref_t elseActionStatement;
};
}
}

#endif // ES_IF_STAMTEN_H
