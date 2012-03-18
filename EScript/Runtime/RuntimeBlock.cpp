// RuntimeBlock.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "RuntimeBlock.h"

#include "../Objects/Object.h"
#include "../Objects/Values/Void.h"
#include "RuntimeContext.h"
#include "Runtime.h"

using namespace EScript;

std::stack<RuntimeBlock *> RuntimeBlock::pool;

//! (static) Factory
RuntimeBlock * RuntimeBlock::create(RuntimeContext * _ctxt,const AST::BlockStatement * staticBlock,RuntimeBlock * _parentRTB){
	RuntimeBlock * rtb=NULL;
#ifdef ES_DEBUG_MEMORY
	rtb = new RuntimeBlock();
#else
	if(pool.empty()){
		rtb=new RuntimeBlock();
	}else{
		rtb=pool.top();
		pool.pop();
	}
#endif
	rtb->init(_ctxt,staticBlock,_parentRTB);
	return rtb;
}
//! static
void RuntimeBlock::release(RuntimeBlock *rtb){
	rtb->localVariables.reset();
	rtb->ctxt=NULL;
#ifdef ES_DEBUG_MEMORY
	delete rtb;
#else
	pool.push(rtb);
#endif
}

// -------------------------------------------------------------------------

//!	(ctor)
RuntimeBlock::RuntimeBlock():
		staticBlock(NULL),ctxt(NULL){
	//ctor
}

//! (dtor)
RuntimeBlock::~RuntimeBlock() {
	//dtor
}

void RuntimeBlock::init(RuntimeContext * _ctxt,const AST::BlockStatement * _staticBlock,RuntimeBlock * parentRTB){
	staticBlock = const_cast<AST::BlockStatement *>(_staticBlock);
	currentStatement = staticBlock->getStatements().begin();
	ctxt = _ctxt;

	if (parentRTB!=NULL) {
		localVariables.init(&parentRTB->localVariables);

	}else{
		localVariables.init();
	}

	 // Initialize the variables declared in the static BlockStatement.
	const AST::BlockStatement::declaredVariableMap_t * staticVars= staticBlock->getVars();
	if(staticVars!=NULL){
		for ( AST::BlockStatement::declaredVariableMap_t::const_iterator it = staticVars->begin();  it != staticVars->end(); ++it) {
			localVariables.declare((*it),Void::get());
		}
	}
}


/*!	changed due to BUG[20090424] */
bool RuntimeBlock::assignToVariable(Runtime & rt,const StringId id,Object * val) {
	// look for local variable
	if (localVariables.findAndUpdate(id,val)){
		return true;
	} else if (ctxt->getCaller()!=NULL ) {
		return rt.assignToAttribute(ctxt->getCaller(),id,val);
	} else {
		return false;
	}

}

void RuntimeBlock::gotoStatement(int pos){
	if(pos>=0){
		currentStatement=staticBlock->getStatements().begin()+static_cast<size_t>(pos);
	}else if(pos==AST::BlockStatement::POS_HANDLE_AND_LEAVE){
		currentStatement = staticBlock->getStatements().end();
	}else if(pos==AST::BlockStatement::POS_DONT_HANDLE){
		currentStatement = staticBlock->getStatements().end();
	}

}
