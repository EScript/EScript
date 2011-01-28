#include "IfControl.h"
#include <iostream>
#include <sstream>
#include "../Runtime/Runtime.h"
using namespace EScript;

IfControl::IfControl(Object * condition,Object * action,Object * elseAction):
        conditionRef(condition),actionRef(action),elseActionRef(elseAction) {
    //ctor
}

IfControl::~IfControl() {
    //dtor
}

std::string IfControl::toString()const {
    std::ostringstream sprinter;
    sprinter << "if("<<conditionRef.toString()<< ")"<<actionRef.toString();
    if (!elseActionRef.isNull())
        sprinter << " else "<<elseActionRef.toString();
    sprinter << " ";
    return sprinter.str();
}

Object * IfControl::execute(Runtime & rt) {
    if (!conditionRef.isNull()) {
        ObjRef conResult=rt.executeObj(conditionRef.get());
        if(!rt.assertNormalState(this)) return NULL;

        if (conResult.toBool()) {
            return actionRef.isNull()?NULL:rt.executeObj(actionRef.get());
        }
    }
    if (!elseActionRef.isNull()) {
        return rt.executeObj(elseActionRef.get());
    }
    return NULL;
}
