#include "RuntimeBlock.h"

#include "../Objects/Void.h"
#include "../Consts.h"
#include "Runtime.h"
#include <iostream>

using namespace EScript;

std::stack<RuntimeBlock *> RuntimeBlock::pool;

/*! (static) Factory */
RuntimeBlock * RuntimeBlock::create(const Block * staticBlock,RuntimeBlock * _parentRTB,const ObjPtr & _caller){
	RuntimeBlock * rtb=NULL;
	if(pool.empty()){
		rtb=new RuntimeBlock();
	}else{
		rtb=pool.top();
		pool.pop();
	}
	rtb->init(staticBlock,_parentRTB,_caller);
	return rtb;
}
/*! static */
void RuntimeBlock::release(RuntimeBlock *rtb){
	pool.push(rtb);
	rtb->localVariables.reset();
	rtb->parentRTB=NULL;
	rtb->callerRef=NULL;
//	static size_t max=0;
//	if(pool.size()>max){
//		std::cout << "\r"<<max;
//		max=pool.size();
//	}
}

// -------------------------------------------------------------------------

/**
 * [ctor]
 */
RuntimeBlock::RuntimeBlock():
        staticBlock(NULL),parentRTB(NULL) {
    //ctor
}

/*!	*/
void RuntimeBlock::init(const Block * _staticBlock,RuntimeBlock * _parentRTB,const ObjPtr & _caller){
	staticBlock = const_cast<Block *>(_staticBlock);
	parentRTB = _parentRTB;
	callerRef=_caller; // setCaller

    if (parentRTB!=NULL) {
		localVariables.init(&parentRTB->localVariables);
		// @BUGFIX: [20070626]
		if (callerRef.isNull() && !parentRTB->callerRef.isNull()) {
			callerRef=parentRTB->callerRef;
		}
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

	if(!callerRef.isNull())
		initLocalVariable( Consts::IDENTIFIER_this,callerRef.get());

}

/**
 * [dtor]
 */
RuntimeBlock::~RuntimeBlock() {
    //dtor
}

/*!	*/
void RuntimeBlock::setCaller(Object * _caller){
	callerRef=_caller;
    if (!callerRef.isNull()) {
        initLocalVariable( Consts::IDENTIFIER_this,callerRef.get());
    }
}

/**
 * // changed for BUG[20090424]
 */
bool RuntimeBlock::assignToVariable(const identifierId id,Object * val) {

    // look for local variable
    if (!localVariables.findAndUpdate(id,val)) {

        // look for member variable
        if (!callerRef.isNull() && callerRef->getAttribute(id)) {
            callerRef->setObjAttribute(id,val);
        } else {
            return false;
//            // define and set new local variable
//            localVariables.set(id,val);
        }
    }
    return true;
}


