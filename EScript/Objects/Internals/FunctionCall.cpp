// FunctionCall.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "FunctionCall.h"
#include "GetAttribute.h"
#include "../../Parser/CompilerContext.h"

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
void FunctionCall::_asm(CompilerContext & ctxt){
//	ctxt.out << "//<FunctionCall '"<<toString()<<"'\n";

	do{
		GetAttribute * gAttr = expRef.toType<GetAttribute>();

		// getAttributeExpression (...)
		if( gAttr ){
			const StringId attrId = gAttr->getAttrId();

			if(gAttr->getObjectExpression()==NULL){ // singleIdentifier (...)
				const int localVarIndex = ctxt.getVarIndex(attrId);
				if(localVarIndex>=0){
					ctxt.addInstruction(Instruction::createPushVoid());
//					ctxt.out << "push NULL\n";
					ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
//					ctxt.out << "getLocalVar $" <<localVarIndex<<"\n";
				}else{
					ctxt.addInstruction(Instruction::createFindVariable(attrId));
//					ctxt.out << "findVar '" <<attrId.toString()<<"'\n";
				}
				break;
			} // getAttributeExpression.identifier (...)
			else if(GetAttribute * gAttrGAttr = dynamic_cast<GetAttribute *>(gAttr->getObjectExpression() )){
				gAttrGAttr->_asm(ctxt);
				ctxt.addInstruction(Instruction::createDup());
//				ctxt.out << "dup\n";
				ctxt.addInstruction(Instruction::createGetAttribute(attrId));
//				ctxt.out << "getAttribute(2) $" <<attrId.toString()<<"\n";
				break;
			} // somethingElse.identifier (...) e.g. foo().bla(), 7.bla()
			else{
				expRef->_asm(ctxt);
				break;
			}
		}else{
			ctxt.addInstruction(Instruction::createPushVoid());
//			ctxt.out << "push NULL\n";
			expRef->_asm(ctxt);
			break;
		}
		
	}while(false);
	
	
//		out<<"\n";
//	}
	for(std::vector<ObjRef>::iterator it=parameters.begin();it!=parameters.end();++it){
		(*it)->_asm(ctxt);
	}
	ctxt.addInstruction(Instruction::createCall(parameters.size()));
//	ctxt.out << "call "<<parameters.size()<<"\n";
	
//	ctxt.out << "//FunctionCall >\n";


}