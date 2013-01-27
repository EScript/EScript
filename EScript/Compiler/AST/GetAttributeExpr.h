// GetAttributeExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
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
