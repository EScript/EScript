// GetAttributeExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_GETATTRIBUTE_EXPR_H
#define ES_GETATTRIBUTE_EXPR_H

#include "../Object.h"
#include <string>

namespace EScript {
namespace AST {

/*! [GetAttributeExpr]  ---|> [Object]  */
class GetAttributeExpr : public Object {
		ES_PROVIDES_TYPE_NAME(GetAttributeExpr)
	public:
		GetAttributeExpr(ObjPtr _obj,StringId _attrId) : objExpression(_obj),attrId(_attrId) {}
		virtual ~GetAttributeExpr(){}

		const StringId & getAttrId()const		{   return attrId;  }
		const std::string & getAttrName()const	{   return attrId.toString();    }
		ObjPtr getObjectExpression()const		{   return objExpression;    }

		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_GET_ATTRIBUTE_EXPRESSION; }


	private:
		ObjRef objExpression;
		StringId attrId;
};
}
}

#endif // ES_GETATTRIBUTE_EXPR_H
