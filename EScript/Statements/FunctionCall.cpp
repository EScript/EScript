#include "FunctionCall.h"
#include "../EScript.h"
#include "../Runtime/Runtime.h"

#include <iostream>
#include <sstream>
#include <iterator>

using namespace EScript;

// \todo change the type of the used array ?

/*!	(ctor)	*/
FunctionCall::FunctionCall(Object * exp,const std::vector<ObjRef> & parameterVec,bool _isConstructorCall,
						identifierId filename,int line/*=-1*/):
        expRef(exp),parameters(parameterVec.begin(), parameterVec.end()),constructorCall(_isConstructorCall),
        lineNumber(line),filenameId(filename){
    //ctor
}

/*!	---|> [Object]	*/
std::string FunctionCall::toString() const {
	std::ostringstream sprinter;
	sprinter << expRef.toString() << "(";
	std::transform(parameters.begin(), parameters.end(), std::ostream_iterator<std::string>(sprinter, ", "), ObjectPrinter());
	sprinter << ")";
	return sprinter.str();
}

/*!	---|> [Object]	*/
std::string FunctionCall::toDbgString() const {
	std::ostringstream sprinter;
	sprinter << expRef.toString() << "(";
	std::transform(parameters.begin(), parameters.end(), std::ostream_iterator<std::string>(sprinter, ", "), ObjectDebugPrinter());
	sprinter << ") [" << getFilename() << ":" << getLine() << "]";
	return sprinter.str();
}

/*!	---|> [Object]	*/
Object * FunctionCall::execute(Runtime & rt) {
	return rt.executeFunctionCall(this);
}
