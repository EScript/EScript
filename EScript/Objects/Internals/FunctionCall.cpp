// FunctionCall.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "FunctionCall.h"
#include "GetAttribute.h"

#include <iterator>
#include <sstream>

using namespace EScript;

// \todo change the type of the used array ?

//! (ctor)
FunctionCall::FunctionCall(Object * exp,const std::vector<ObjRef> & parameterVec,bool _isConstructorCall,
						StringId filename,int line/*=-1*/):
		expRef(exp),parameters(parameterVec.begin(), parameterVec.end()),constructorCall(_isConstructorCall),
		lineNumber(line),filenameId(filename){
	//ctor
}

//! ---|> [Object]
std::string FunctionCall::toString() const {
	std::ostringstream sprinter;
	sprinter << expRef.toString() << "(";
	if(!parameters.empty()){
		std::vector<ObjRef>::const_iterator it = parameters.begin();
		sprinter<< (*it)->toDbgString();
		for(++it;it!=parameters.end();++it)
			sprinter<<", "<< (*it)->toDbgString();
	}
	sprinter << ")";
	return sprinter.str();
}

//! ---|> [Object]
std::string FunctionCall::toDbgString() const {
	std::ostringstream sprinter;
	sprinter << expRef.toString() << "(";
	if(!parameters.empty()){
		std::vector<ObjRef>::const_iterator it = parameters.begin();
		sprinter<< (*it)->toDbgString();
		for(++it;it!=parameters.end();++it)
			sprinter<<", "<< (*it)->toDbgString();
	}

	sprinter << ") near '" << getFilename() << "':" << getLine() << "";
	return sprinter.str();
}

//! ---|> Statement
void FunctionCall::_asmOut(std::ostream & out){
	out << "//<FunctionCall '"<<toString()<<"'\n";
//	if(expRef.isNotNull()){ 
	// switch by type: getVar -> findVar, function call? add push[NULL] , else 'dup'
	
	do{
		GetAttribute * gAttr = expRef.toType<GetAttribute>();

		// getAttributeExpression (...)
		if( gAttr ){
			if(gAttr->getObjectExpression()==NULL){ // singleIdentifier (...)
				out << "findVar $" <<gAttr->getAttrId().toString()<<"\n";
				break;
			} // getAttributeExpression.identifier (...)
			else if(GetAttribute * gAttrGAttr = dynamic_cast<GetAttribute *>(gAttr->getObjectExpression() )){
				gAttrGAttr->_asmOut(out);
				out << "dup\n";
				out << "getAttribute(2) $" <<gAttr->getAttrId().toString()<<"\n";
				break;
			} // somethingElse.identifier (...) e.g. foo().bla(), 7.bla()
			else{
				expRef->_asmOut(out);
				break;
			}
		}else{
			out << "push NULL\n";
			expRef->_asmOut(out);
			break;
		}
		
	}while(false);
	
	
//		out<<"\n";
//	}
	for(std::vector<ObjRef>::iterator it=parameters.begin();it!=parameters.end();++it){
		(*it)->_asmOut(out);
	}
	out << "call "<<parameters.size()<<"\n";
	
	out << "//FunctionCall >\n";
//	out << "push $" <<attrId.toString()<<"\n";

}