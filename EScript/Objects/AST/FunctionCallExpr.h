// FunctionCallExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "../Object.h"
#include <vector>

namespace EScript {
namespace AST {
	
/*! [FunctionCallExpr]  ---|> [Object]  */
class FunctionCallExpr : public Object {
		ES_PROVIDES_TYPE_NAME(FunctionCallExpr)
	public:
		static FunctionCallExpr * createConstructorCall(Object * objExpr,const std::vector<ObjRef> & parameterExpr,StringId filename=StringId(), int line=-1){
			return new FunctionCallExpr(objExpr,parameterExpr,true,filename,line);
		}
		
		static FunctionCallExpr * createFunctionCall(Object * getFunctionExpr,const std::vector<ObjRef> & parameterExpr,StringId filename=StringId(), int line=-1){
			return new FunctionCallExpr(getFunctionExpr,parameterExpr,false,filename,line);
		}
		virtual ~FunctionCallExpr() {}

		//! only valid if constructorCall == false
		ObjPtr getGetFunctionExpression()const    		{   return expRef;    }

		//! only valid if constructorCall == true
		ObjPtr getObjectExpression()const    			{   return expRef;    }

		int getLine()const        						{   return lineNumber;  }
		void setLine(int i)      						{   lineNumber = i;   }
		void setFilename(const std::string & filename)  {   filenameId = filename;  }
		std::string getFilename()const                  {   return filenameId.toString();    }

		bool isConstructorCall()const					{	return constructorCall; }
		const std::vector<ObjRef> & getParams()const	{	return parameters; }
		size_t getNumParams()const						{	return parameters.size(); }
		Object * getParamExpression(size_t i)const		{	return parameters[i].get();	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_FUNCTION_CALL_EXPRESSION; }
		
	protected:
		
		FunctionCallExpr(Object * exp,const std::vector<ObjRef> & _parameters,bool _isConstructorCall,StringId filename, int line) :
				expRef(exp),parameters(_parameters),constructorCall(_isConstructorCall),lineNumber(line),filenameId(filename){
		}

		ObjRef expRef;
		std::vector<ObjRef> parameters;
		bool constructorCall;
		int lineNumber; // for debugging
		StringId filenameId; // for debugging
};
}
}

#endif // FUNCTIONCALL_H
