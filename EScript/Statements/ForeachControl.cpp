#include "ForeachControl.h"
#include "FunctionCall.h"
#include "GetAttribute.h"

#include "../Objects/Iterator.h"
#include "../Runtime/Runtime.h"
#include "../Consts.h"
#include <sstream>


using namespace EScript;

/*!	(ctor)	*/
ForeachControl::ForeachControl(Object * arrayExpression,identifierId _keyVar,identifierId _valueVar,Object * action):
        keyVar(_keyVar),valueVar(_valueVar),actionRef(action) {
    arrayExpressionRef=new FunctionCall(new GetAttribute(arrayExpression,Consts::IDENTIFIER_fn_getIterator), std::vector<ObjRef>()); // TODO!!!
    //ctor
}

/*!	(dtor)	*/
ForeachControl::~ForeachControl() {
    //dtor
}

/*!	---|> [Object]	*/
std::string ForeachControl::toString()const {
    std::ostringstream sprinter;
    sprinter<<"foreach("<<arrayExpressionRef.toString()<<" as ";
    if (keyVar) sprinter<<EScript::identifierIdToString(keyVar)<<", ";
    if (valueVar) sprinter<<EScript::identifierIdToString(valueVar);
    sprinter<<") "<<actionRef.toString();
    return sprinter.str();
}

/*!	---|> [Object]	*/
Object * ForeachControl::execute(Runtime & rt) {
    ERef<Iterator> itRef=dynamic_cast<Iterator *>(rt.executeObj(arrayExpressionRef.get()));
    if(!rt.assertNormalState(this) || itRef.isNull())
		return NULL;

    ObjRef resultRef;

    while (!itRef->end()) {
        if (keyVar)
            rt.assignToVariable(keyVar,itRef->key());
        if (valueVar)
            rt.assignToVariable(valueVar,itRef->value());

        if(!actionRef.isNull()){
            try {
                resultRef=rt.executeObj(actionRef.get());
            } catch (Object * e) {
                std::cout << "###"<<e<<itRef->key();
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
        }

        itRef->next();
    }
    return resultRef.detachAndDecrease();
}
