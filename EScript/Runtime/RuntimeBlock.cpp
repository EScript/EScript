// RuntimeBlock.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "RuntimeBlock.h"

#include "../Objects/Void.h"
#include "RuntimeContext.h"
#include <iostream>

using namespace EScript;

std::stack<RuntimeBlock *> RuntimeBlock::pool;

//! (static) Factory
RuntimeBlock * RuntimeBlock::create(RuntimeContext * _ctxt,const Block * staticBlock,RuntimeBlock * _parentRTB){
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

void RuntimeBlock::init(RuntimeContext * _ctxt,const Block * _staticBlock,RuntimeBlock * parentRTB){
	staticBlock = const_cast<Block *>(_staticBlock);
	currentStatement = staticBlock->getStatements().begin();
	ctxt = _ctxt;

	if (parentRTB!=NULL) {
		localVariables.init(&parentRTB->localVariables);

	}else{
		localVariables.init();
	}

	 // Initialize the variables declared in the static Block.
	const Block::declaredVariableMap_t * staticVars= staticBlock->getVars();
	if(staticVars!=NULL){
		for ( Block::declaredVariableMap_t::const_iterator it = staticVars->begin();  it != staticVars->end(); ++it) {
			localVariables.declare((*it),Void::get());
		}
	}
}


/*!	changed due to BUG[20090424] */
bool RuntimeBlock::assignToVariable(const identifierId id,Object * val) {
	// look for local variable
	if (localVariables.findAndUpdate(id,val)){
		return true;
	} else if (ctxt->getCaller()!=NULL ) {
		return ctxt->getCaller()->assignAttribute(id,val);
	} else {
		return false;
	}

}

void RuntimeBlock::gotoStatement(int pos){
	if(pos>=0){
		currentStatement=staticBlock->getStatements().begin()+static_cast<size_t>(pos);
	}else if(pos==Block::POS_HANDLE_AND_LEAVE){
		currentStatement = staticBlock->getStatements().end();
	}else if(pos==Block::POS_DONT_HANDLE){
		currentStatement = staticBlock->getStatements().end();
	}

}

