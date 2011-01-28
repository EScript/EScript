#include "LogicOp.h"
#include "../Objects/Bool.h"
#include "../Runtime/Runtime.h"
#include <sstream>

using namespace EScript;

LogicOp::LogicOp(Object * _left,Object * _right,opType_T _op):
        leftRef(_left),rightRef(_right),op(_op) {
    //ctor
}

LogicOp::~LogicOp() {
    //dtor
}

std::string LogicOp::toString()const {
    std::ostringstream sprinter;
    if (op==NOT)
		sprinter<<"!";
    if (!leftRef.isNull())
		sprinter<<leftRef.toString();
    if (op==OR)
		sprinter<<"||";
    else if (op==AND)
		sprinter<<"&&";
    if (!rightRef.isNull())
		sprinter<<rightRef.toString();
    return sprinter.str();
}

Object * LogicOp::execute(Runtime & rt) {

    ObjRef resultRef=rt.executeObj(getLeft());
    if(!rt.assertNormalState(this))
		return NULL;

    bool b=resultRef.toBool();

    if (op==NOT) {
        resultRef=Bool::create(!b);
        return resultRef.detachAndDecrease();
    } else if (b && op==OR) {
        resultRef=Bool::create(true);
        return resultRef.detachAndDecrease();
    } else if (!b && op==AND) {
        resultRef=Bool::create(false);
        return resultRef.detachAndDecrease();
    }
    resultRef=rt.executeObj(getRight());
    if(!rt.assertNormalState(this))
		return NULL;

    b=resultRef.toBool();

    resultRef=Bool::create(b);
    return resultRef.detachAndDecrease();
}
