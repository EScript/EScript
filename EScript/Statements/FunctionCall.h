// FunctionCall.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include "../Object.h"
#include <vector>

namespace EScript {

/*! [FunctionCall]  ---|> [Object]  */
class FunctionCall : public Object {
		ES_PROVIDES_TYPE_NAME(FunctionCall)
	public:
		FunctionCall(Object * exp,const std::vector<ObjRef> & parameter,bool isConstructorCall=false,
					identifierId filename=0, int line=-1);
		virtual ~FunctionCall() {}

		Object * getStatement()const    			{   return expRef.get();    }

		int getLine()const        					{   return lineNumber;  }
		void setLine(int i)      					{   lineNumber=i;   }
		void setFilename(const std::string & filename)  {   filenameId=stringToIdentifierId(filename);  }
		std::string getFilename()const                  {   return identifierIdToString(filenameId);    }

		bool isConstructorCall()const				{	return constructorCall; }
		size_t getNumParams()const					{	return parameters.size(); }
		Object * getParamExpression(size_t i)const	{	return parameters[i].get();	}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual std::string toDbgString()const;
		virtual Object * execute(Runtime & rt);

	protected:
		ObjRef expRef;
		std::vector<ObjRef> parameters;
		bool constructorCall;
		int lineNumber; // for debugging
		identifierId filenameId; // for debugging
};
}

#endif // FUNCTIONCALL_H
