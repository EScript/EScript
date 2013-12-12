// SetAttributeExpr.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_SETATTRIBUTE_H
#define ES_SETATTRIBUTE_H

#include "ASTNode.h"
#include "../../Utils/Attribute.h"
#include <string>

namespace EScript {
namespace AST {

//! [SetAttributeExpr]  ---|> [ASTNode]
class SetAttributeExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(SetAttributeExpr)
	public:
		static SetAttributeExpr * createAssignment(ptr_t obj,StringId attrId,ptr_t valueExp,int _line=-1){
			SetAttributeExpr * sa  = new SetAttributeExpr(obj,attrId,valueExp,0,_line) ;
			sa->assign = true;
			return sa;
		}

		SetAttributeExpr(ptr_t obj,StringId _attrId,ptr_t _valueExp,Attribute::flag_t _attrFlags,int _line=-1) :
			ASTNode(TYPE_SET_ATTRIBUTE_EXPRESSION,_line),
			objExpr(obj),valueExpr(_valueExp),attrId(_attrId),attrFlags(_attrFlags),assign(false) {}

		virtual ~SetAttributeExpr(){}

		const StringId & getAttrId()const				{	return attrId;	}
		ptr_t getObjectExpression()const				{	return objExpr;	}
		Attribute::flag_t getAttributeProperties()const	{	return attrFlags;	}
		ptr_t getValueExpression()const					{	return valueExpr;	}
		std::string getAttrName()const					{	return attrId.toString();	}
		bool isAssignment()const						{	return assign;	}

	private:
		friend class EScript::Runtime;
		ref_t objExpr;
		ref_t valueExpr;
		StringId attrId;
		Attribute::flag_t attrFlags;
		bool assign;
};
}
}

#endif // ES_SETATTRIBUTE_H
