// GetAttributeExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef GETATTRIBUTE_H
#define GETATTRIBUTE_H

#include "../Object.h"
#include <string>

namespace EScript {
namespace AST {
	
/*! [GetAttributeExpr]  ---|> [Object]  */
class GetAttributeExpr : public Object {
		ES_PROVIDES_TYPE_NAME(GetAttributeExpr)
	public:
		GetAttributeExpr(Object * obj,StringId attrId);
		GetAttributeExpr(Object * obj,const std::string & attrName);
		virtual ~GetAttributeExpr();

		StringId getAttrId()const				{   return attrId;  }
		const std::string & getAttrName()const	{   return attrId.toString();    }
		Object * getObjectExpression()			{   return objRef.get();    }

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_GET_ATTRIBUTE_EXPRESSION; }

		//! ---|> Statement
		virtual void _asm(CompilerContext & ctxt);
	private:
		ObjRef objRef;
		StringId attrId;
};
}
}

#endif // GETATTRIBUTE_H
