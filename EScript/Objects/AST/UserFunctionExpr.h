// UserFunctionExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_USERFUNCTION_EXPR_H
#define ES_USERFUNCTION_EXPR_H

#include "../ExtObject.h"
#include "../../Utils/CodeFragment.h"
#include <vector>

namespace EScript {
namespace AST{
class BlockExpr;
}
class String;

/*! [UserFunctionExpr]  ---|> [ExtObject]	*/
class UserFunctionExpr : public ExtObject {
		ES_PROVIDES_TYPE_NAME(UserFunctionExpr)
	public:
	// -------------------------------------------------------------

	/*! @name Parameter */
	//	@{

		/*! [Parameter] */
		class Parameter {
			private:
				StringId name;
				ObjRef defaultValueExpressionRef;
				std::vector<ObjRef> typeExpressions;
				bool multiParam;
			public:
				Parameter(const StringId & name,ObjPtr defaultValueExpression,std::vector<ObjRef> & _typeExpressions);

				StringId getName()const									{   return name;    }
				const std::vector<ObjRef>  & getTypeExpressions()const	{   return typeExpressions;   }

				void setMultiParam(bool b)								{   multiParam=b;   }
				bool isMultiParam()const								{   return multiParam;  }

				ObjPtr getDefaultValueExpression()const {
					return defaultValueExpressionRef;
				}
				void setDefaultValueExpression(Object * newDefaultExpression) {
					defaultValueExpressionRef=newDefaultExpression;
				}

		};

		typedef std::vector<Parameter> parameterList_t;
	//	@}

	// -------------------------------------------------------------

	/*! @name Main */
	//	@{
		UserFunctionExpr(AST::BlockExpr * block,const std::vector<ObjRef> & _sConstrExpressions,int line);
		virtual ~UserFunctionExpr() {}

		AST::BlockExpr * getBlock()const					{	return blockRef.get();	}
		const CodeFragment & getCode()const					{	return code;	}
		int getLine()const									{	return line;	}
		int getMaxParamCount()const;
		int getMinParamCount()const;
		const parameterList_t & getParamList()const			{	return params;	}
		parameterList_t & getParamList()					{	return params;	}

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}
		const std::vector<ObjRef> & getSConstructorExpressions()const	{	return sConstrExpressions;	}

		void setCode(const CodeFragment & _code)			{	code = _code;	}
	
		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION_EXPRESSION;	}

	private:
		ERef<AST::BlockExpr> blockRef;
		parameterList_t params; 
		std::vector<ObjRef> sConstrExpressions;
		CodeFragment code;
		int line;
	//	@}
};
}

#endif // ES_USERFUNCTION_EXPR_H
