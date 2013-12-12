// GetAttributeExpr.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_GETATTRIBUTE_EXPR_H
#define ES_GETATTRIBUTE_EXPR_H

#include "ASTNode.h"
#include <string>

namespace EScript {
namespace AST {

//! [GetAttributeExpr]  ---|> [ASTNode]
class GetAttributeExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(GetAttributeExpr)
	public:
		GetAttributeExpr(ptr_t _obj,StringId _attrId) :
				ASTNode(TYPE_GET_ATTRIBUTE_EXPRESSION,true),objExpression(_obj),attrId(_attrId) {}
		virtual ~GetAttributeExpr(){}

		const StringId & getAttrId()const			{	return attrId;	}
		const std::string & getAttrName()const		{	return attrId.toString();	}
		ptr_t getObjectExpression()const			{	return objExpression;	}

	private:
		ref_t objExpression;
		StringId attrId;
};
}
}

#endif // ES_GETATTRIBUTE_EXPR_H
