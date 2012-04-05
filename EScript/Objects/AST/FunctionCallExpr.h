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

		static FunctionCallExpr * createSysCall(uint32_t sysCallId,const std::vector<ObjRef> & parameterExpr,StringId filename=StringId(), int line=-1){
			return new FunctionCallExpr(sysCallId,parameterExpr,filename,line);
		}
		virtual ~FunctionCallExpr() {}

		//! only valid if constructorCall == false and sysCall == false
		ObjPtr getGetFunctionExpression()const    		{   return expRef;    }

		//! only valid if constructorCall == true
		ObjPtr getObjectExpression()const    			{   return expRef; }

		//! only valid if sysCall == true
		uint32_t getSysCallId()const    				{   return sysCallId;    }

		int getLine()const        						{   return lineNumber;  }
		void setLine(int i)      						{   lineNumber = i;   }
		void setFilename(const std::string & filename)  {   filenameId = filename;  }
		std::string getFilename()const                  {   return filenameId.toString();    }

		bool isConstructorCall()const					{	return constructorCall; }
		bool isSysCall()const							{	return sysCall; }
		const std::vector<ObjRef> & getParams()const	{	return parameters; }
		size_t getNumParams()const						{	return parameters.size(); }
		Object * getParamExpression(size_t i)const		{	return parameters[i].get();	}

		/// ---|> [Object]
		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_FUNCTION_CALL_EXPRESSION; }

	protected:

		FunctionCallExpr(Object * exp,const std::vector<ObjRef> & _parameters,bool _isConstructorCall,StringId filename, int line) :
				expRef(exp),parameters(_parameters),constructorCall(_isConstructorCall),sysCall(false),sysCallId(0),
				lineNumber(line),filenameId(filename){
		}

		FunctionCallExpr(uint32_t _sysCallId,const std::vector<ObjRef> & _parameters,StringId filename, int line) :
				expRef(NULL),parameters(_parameters),constructorCall(false),sysCall(true),sysCallId(_sysCallId),
				lineNumber(line),filenameId(filename){
		}

		ObjRef expRef;
		std::vector<ObjRef> parameters;
		bool constructorCall;
		bool sysCall;
		uint32_t sysCallId;
		int lineNumber; // for debugging
		StringId filenameId; // for debugging
};
}
}

#endif // FUNCTIONCALL_H
