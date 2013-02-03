// FunctionCallExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_FUNCTIONCALL_EXPR_H
#define ES_FUNCTIONCALL_EXPR_H

#include "ASTNode.h"
#include <vector>

namespace EScript {
namespace AST {

//! [FunctionCallExpr]  ---|> [ASTNode]
class FunctionCallExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(FunctionCallExpr)
	public:
		static FunctionCallExpr * createConstructorCall(ptr_t objExpr,const refArray_t & parameterExpr, int line=-1){
			return new FunctionCallExpr(objExpr,parameterExpr,true,line);
		}

		static FunctionCallExpr * createFunctionCall(ptr_t getFunctionExpr,const refArray_t & parameterExpr, int line=-1){
			return new FunctionCallExpr(getFunctionExpr,parameterExpr,false,line);
		}

		static FunctionCallExpr * createSysCall(uint32_t sysCallId,const refArray_t & parameterExpr, int line=-1){
			return new FunctionCallExpr(sysCallId,parameterExpr,line);
		}
		virtual ~FunctionCallExpr() {}

		//! only valid if constructorCall == false and sysCall == false
		ptr_t getGetFunctionExpression()const			{	return expRef;	}

		//! only valid if constructorCall == true
		ptr_t getObjectExpression()const				{	return expRef; }

		//! only valid if sysCall == true
		uint32_t getSysCallId()const					{	return sysCallId;	}

		bool isConstructorCall()const					{	return constructorCall; }
		bool isSysCall()const							{	return sysCall; }
		const refArray_t & getParams()const	{	return parameters; }
		size_t getNumParams()const						{	return parameters.size(); }
		ptr_t getParamExpression(size_t i)const			{	return parameters[i].get();	}

		const std::vector<uint32_t>& getExpandingParameters()const	{	return expandingParameters;	}
		bool hasExpandingParameters()const							{	return !expandingParameters.empty();	}
		void emplaceExpandingParameters(std::vector<uint32_t>&&v)	{	expandingParameters = v;	}

	protected:

		FunctionCallExpr(ptr_t exp,const refArray_t & _parameters,bool _isConstructorCall, int _line) :
				ASTNode(TYPE_FUNCTION_CALL_EXPRESSION,true,_line),
				expRef(exp),parameters(_parameters),constructorCall(_isConstructorCall),sysCall(false),sysCallId(0){
		}

		FunctionCallExpr(uint32_t _sysCallId,const refArray_t & _parameters, int _line) :
				ASTNode(TYPE_FUNCTION_CALL_EXPRESSION,true,_line),
				expRef(nullptr),parameters(_parameters),constructorCall(false),sysCall(true),sysCallId(_sysCallId){
		}

		ref_t expRef;
		refArray_t parameters;
		bool constructorCall;
		bool sysCall;
		uint32_t sysCallId;
		std::vector<uint32_t> expandingParameters;
};
}
}

#endif // ES_FUNCTIONCALL_EXPR_H
