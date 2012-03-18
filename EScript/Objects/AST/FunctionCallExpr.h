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
		FunctionCallExpr(Object * exp,const std::vector<ObjRef> & parameter,bool isConstructorCall=false,
					StringId filename=StringId(), int line=-1);
		virtual ~FunctionCallExpr() {}

		Object * getStatement()const    				{   return expRef.get();    }

		int getLine()const        						{   return lineNumber;  }
		void setLine(int i)      						{   lineNumber = i;   }
		void setFilename(const std::string & filename)  {   filenameId = filename;  }
		std::string getFilename()const                  {   return filenameId.toString();    }

		bool isConstructorCall()const					{	return constructorCall; }
		size_t getNumParams()const						{	return parameters.size(); }
		Object * getParamExpression(size_t i)const		{	return parameters[i].get();	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual std::string toDbgString()const;
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_FUNCTION_CALL_EXPRESSION; }
		
		//! ---|> Statement
		virtual void _asm(CompilerContext & ctxt);
	protected:
		ObjRef expRef;
		std::vector<ObjRef> parameters;
		bool constructorCall;
		int lineNumber; // for debugging
		StringId filenameId; // for debugging
};
}
}

#endif // FUNCTIONCALL_H
