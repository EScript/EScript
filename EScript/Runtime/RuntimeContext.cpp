// RuntimeContext.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "RuntimeContext.h"

#include "../Consts.h"

using namespace EScript;

std::stack<RuntimeContext *> RuntimeContext::pool;

//! (static) Factory
RuntimeContext * RuntimeContext::create(){
	RuntimeContext * rts=NULL;
	if(pool.empty()){
		rts=new RuntimeContext();
	}else{
		rts=pool.top();
		pool.pop();
	}
	rts->init();
	return rts;
}

//! static
void RuntimeContext::release(RuntimeContext *rts){
	pool.push(rts);
	rts->caller=NULL;
	while(!rts->runtimeBlockStack.empty())
		rts->runtimeBlockStack.pop();
//	static size_t max=0;
//	if(pool.size()>max){
//		std::cout << "\r"<<max;
//		max=pool.size();
//	}
}

// -------------------------------------------------------------------------

//!	(ctor)
RuntimeContext::RuntimeContext(){
	//ctor
}

//! (dtor)
RuntimeContext::~RuntimeContext() {
	//dtor
}

void RuntimeContext::init(){
	caller=NULL;
	pushRTB(NULL); // push endMarking, so that getCurrentRTB() never accesses an invalid entry.
}

RuntimeBlock * RuntimeContext::createAndPushRTB(const Block * staticBlock){
	RuntimeBlock * newRTB = RuntimeBlock::create(this,staticBlock,getCurrentRTB());
	pushRTB(newRTB);
	return newRTB;
}

// assumes that the stack contains exactly one RTB
void RuntimeContext::initCaller(const ObjPtr &  obj){
	caller = obj;
	if (!caller.isNull()) {
		getCurrentRTB()->initLocalVariable( Consts::IDENTIFIER_this,caller.get());
	}
}
