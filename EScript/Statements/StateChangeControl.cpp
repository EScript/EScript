#include "StateChangeControl.h"
#include "../Runtime/Runtime.h"
#include <sstream>

using namespace EScript;

StateChangeControl * StateChangeControl::createBreakControl(){
    return new StateChangeControl(Runtime::STATE_BREAK,NULL);
}
StateChangeControl * StateChangeControl::createContinueControl(){
    return new StateChangeControl(Runtime::STATE_CONTINUE,NULL);
}
StateChangeControl * StateChangeControl::createReturnControl(Object * expression){
    return new StateChangeControl(Runtime::STATE_RETURN,expression);
}
StateChangeControl * StateChangeControl::createThrowControl(Object * expression){
    return new StateChangeControl(Runtime::STATE_EXCEPTION,expression);
}
StateChangeControl * StateChangeControl::createExitControl(Object * expression){
    return new StateChangeControl(Runtime::STATE_EXIT,expression);
}

StateChangeControl::StateChangeControl(Runtime::state_t _type,Object * exp):
		resultExpRef(exp),type(_type) {
    //ctor
}

StateChangeControl::~StateChangeControl() {
    //dtor
}

std::string StateChangeControl::toString()const {
    std::ostringstream sprinter;
    switch(type){
        case Runtime::STATE_BREAK:{
            sprinter<<"break";
            break;
        }
        case Runtime::STATE_CONTINUE:{
            sprinter<<"continue";
            break;
        }
        case Runtime::STATE_RETURN:{
            sprinter<<"retun";
            break;
        }
        case Runtime::STATE_EXCEPTION:{
            sprinter<<"throw";
            break;
        }
        case Runtime::STATE_EXIT:{
            sprinter<<"exit";
            break;
        }
        default:{
        }
    }
    if( !resultExpRef.isNull())
        sprinter<<" "<<resultExpRef.toString();
    return sprinter.str();
}

Object * StateChangeControl::execute(Runtime & rt) {
    switch(type){
        case Runtime::STATE_BREAK:{
            rt.setState(Runtime::STATE_BREAK);
            return NULL;
        }
        case Runtime::STATE_CONTINUE:{
            rt.setState(Runtime::STATE_CONTINUE);
            return NULL;
        }
        case Runtime::STATE_RETURN:{
            ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
            if(!rt.assertNormalState(this)) return NULL;
            rt.setReturnState(valueRef);
            return NULL;
        }
        case Runtime::STATE_EXCEPTION:{
            ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
            if(!rt.assertNormalState(this)) return NULL;
            rt.setExceptionState(valueRef);
            return NULL;
        }
        case Runtime::STATE_EXIT:{
            ObjRef valueRef=resultExpRef.isNull()?NULL:rt.executeObj(resultExpRef.get());
            if(!rt.assertNormalState(this)) return NULL;
            rt.setExitState(valueRef);
            return NULL;
        }
        default:{
        }
    }
    return NULL;
}
