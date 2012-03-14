// ForeachExpression.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "ForeachExpression.h"
#include "../../Parser/CompilerContext.h"

using namespace EScript;

//! (ctor)
ForeachExpression::ForeachExpression( Object * _collectionExpression,
						const StringId _keyId,
						const StringId _valueId,
						const Statement & _action ) : 
		collectionExpression(_collectionExpression),keyName(_keyId),valueName(_valueId),action(_action){
	//ctor
}

//! ---|> Statement
void ForeachExpression::_asm(CompilerContext & ctxt){
	const uint32_t loopBegin = ctxt.createMarker();
	const uint32_t loopEndMarker = ctxt.createMarker();
	
	////	/* NEW (idea)
////		foreach( [array] as [keyIdent],[valueIndent] ) [action]
////		
////		--> 
////		for( {var __it = [array].getIterator(); var __key = __it.key; var __value = __it.value; var __end = __it.end; var __next = __it.next } ; 
////				! (__it->end) () ; (__it->next)() ){
////			var keyIdent = (__it->key)();
////			var valueIndent = (__it->value)();
////			
////		}
////		
////	*/
	ctxt.out << "//<ForeachExpression '"<<toString()<<"'\n";
//	if(initStmt.isValid()){
//		initStmt._asm(ctxt);
//	}
//	ctxt.out << loopBegin<<":\n";
//	if(preConditionExpression.isNotNull()){
//		preConditionExpression->_asm(ctxt);
//		ctxt.out << "jmpOnFalse " << loopEndMarker << ":\n";
//	}
//	action._asm(ctxt);
//	
//	if(postConditionExpression.isNotNull()){ // increaseStmt is ignored!
//		postConditionExpression->_asm(ctxt);
//		ctxt.out << "jmpOnTrue " << loopBegin << ":\n";
//	}else{
//		if(increaseStmt.isValid()){
//			increaseStmt._asm(ctxt);
//		}
//		ctxt.out << "jmp " << loopBegin << ":\n";
//	}
//	
//
//	ctxt.out << loopEndMarker << ":\n";

	ctxt.out << "//ForeachExpression>\n";

}