#include "ForControl.h"

#include "../Runtime/Runtime.h"
#include <sstream>

using namespace EScript;

ForControl::ForControl(Object * initExpression,Object * condExpression,Object * incrExpression,Object * action):
        initExpressionRef(initExpression),condExpressionRef(condExpression),incrExpressionRef(incrExpression),actionRef(action) {
    //ctor
}

ForControl::~ForControl() {
    //dtor
}

std::string ForControl::toString()const {
    std::ostringstream sprinter;
    sprinter<<"for("<<initExpressionRef.toString()<<
        ";"<<condExpressionRef.toString()<<";"<<
        incrExpressionRef.toString()<<") "<<actionRef.toString();
    return sprinter.str();
}

Object * ForControl::execute(Runtime & rt) {
    if(! initExpressionRef.isNull() ){
        ObjRef initResult=rt.executeObj( initExpressionRef.get());
        if(!rt.assertNormalState(this)) return NULL;
        initResult=NULL;
    }
    if (condExpressionRef.isNull())
        return NULL;

    ObjRef result;

    while (true) {
        ObjRef condResult=rt.executeObj(condExpressionRef.get());
        if(!rt.assertNormalState(this)) return NULL;

        if (!condResult.toBool())
            break;

        result=NULL;
        try {
            result=actionRef.isNull()?NULL:rt.executeObj(actionRef.get());
        } catch (Object * e) {
            throw(e);
        }
        if(!rt.checkNormalState()){
            if(rt.getState()==Runtime::STATE_BREAK){
                rt.resetState();
                break;
            }else if(rt.getState()==Runtime::STATE_CONTINUE){
                rt.resetState();
            }else{
                return NULL;
            }
        }
        ObjRef incrResult=incrExpressionRef.isNull()?NULL:rt.executeObj(incrExpressionRef.get());
        if(!rt.assertNormalState(this)) return NULL;
        incrResult=NULL;
    }
    return result.detachAndDecrease();
}
