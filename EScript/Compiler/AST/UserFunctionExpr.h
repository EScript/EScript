// UserFunctionExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_USERFUNCTION_EXPR_H
#define ES_USERFUNCTION_EXPR_H

#include "ASTNode.h"
#include "../../Utils/CodeFragment.h"
#include <vector>

namespace EScript {
namespace AST{
class Block;


//! [UserFunctionExpr]  ---|> [ExtObject]
class UserFunctionExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(UserFunctionExpr)
	public:
	// -------------------------------------------------------------

	//! @name Parameter
	//	@{

		//! [Parameter]
		class Parameter {
			private:
				StringId name;
				ref_t defaultValueExpressionRef;
				refArray_t typeExpressions;
				bool multiParam;
			public:
				Parameter(const StringId & name,ptr_t defaultValueExpression,refArray_t && _typeExpressions);
				StringId getName()const							{	return name;	}
				const refArray_t  & getTypeExpressions()const	{	return typeExpressions;	}

				void setMultiParam(bool b)						{	multiParam = b;	}
				bool isMultiParam()const						{	return multiParam;	}

				ptr_t getDefaultValueExpression()const			{	return defaultValueExpressionRef;	}
				void setDefaultValueExpression(ptr_t newDefaultExpression) {
					defaultValueExpressionRef = newDefaultExpression;
				}

		};

		typedef std::vector<Parameter> parameterList_t;
	//	@}

	// -------------------------------------------------------------

	//! @name Main
	//	@{
		UserFunctionExpr(Block * block,const refArray_t & _sConstrExpressions,int line);
		virtual ~UserFunctionExpr() {}
		
		Block * getBlock()const									{	return blockRef.get();	}
		const CodeFragment & getCode()const						{	return code;	}

		const parameterList_t & getParamList()const				{	return params;	}
		void emplaceParameterExpressions(parameterList_t && _params){	params = std::move(_params);	}

		refArray_t & getSConstructorExpressions()				{	return sConstrExpressions;	}
		const refArray_t & getSConstructorExpressions()const	{	return sConstrExpressions;	}

		void setCode(const CodeFragment & _code)				{	code = _code;	}

	private:
		ERef<Block> blockRef;
		parameterList_t params;
		refArray_t sConstrExpressions;
		CodeFragment code;
	//	@}
};
}
}

#endif // ES_USERFUNCTION_EXPR_H
