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
class BlockStatement;
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
				Parameter(const StringId & name,Object * defaultValueExpression,std::vector<ObjRef> & _typeExpressions);

				StringId getName()const									{   return name;    }
				const std::vector<ObjRef>  & getTypeExpressions()const	{   return typeExpressions;   }

				void setMultiParam(bool b)								{   multiParam=b;   }
				bool isMultiParam()const								{   return multiParam;  }

				Object * getDefaultValueExpression()const {
					return defaultValueExpressionRef.get();
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
		UserFunctionExpr(AST::BlockStatement * block);
		UserFunctionExpr(AST::BlockStatement * block,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunctionExpr() {}

		void setBlock(AST::BlockStatement * block);
		AST::BlockStatement * getBlock()const				{	return blockRef.get();	}
		int getLine()const;
		const parameterList_t & getParamList()const			{	return params;	}
		parameterList_t & getParamList()					{	return params;	}

		std::vector<ObjRef> & getSConstructorExpressions() 	{	return sConstrExpressions;	}
		const std::vector<ObjRef> & getSConstructorExpressions()const	{	return sConstrExpressions;	}

		void setCode(const CodeFragment & _code)			{	code = _code;	}
		const CodeFragment & getCode()const					{	return code;	}
		int getMaxParamCount()const;
		int getMinParamCount()const;
	
		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_USER_FUNCTION_EXPRESSION;	}

	private:
		ERef<AST::BlockStatement> blockRef;
		parameterList_t params; // \todo should this really saved as a pointer???
		std::vector<ObjRef> sConstrExpressions;

		CodeFragment code;
	//	@}
};
}

#endif // ES_USERFUNCTION_EXPR_H
