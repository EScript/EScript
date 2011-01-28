#include "SetAttribute.h"
#include "../Runtime/Runtime.h"
#include "../Objects/Void.h"
#include <iostream>

using namespace EScript;

/*!	(ctor)	*/
SetAttribute::SetAttribute(Object * obj,identifierId _attrId,Object * valueExp,assignType_t _assignType):
        objRef(obj),valueExpRef(valueExp),attrId(_attrId),assignType(_assignType) {
    //ctor
}

/*!	(dtor)	*/
SetAttribute::~SetAttribute() {
    //dtor
}

/*!	---|> [Object]	*/
std::string SetAttribute::toString()const {
    std::string s="";
    if (!objRef.isNull()) {
        s+=objRef.toString();
    } else s+="_";
    s+="."+identifierIdToString(attrId);
    switch(assignType){
		case ASSIGN:
			s+="=";
			break;
		case SET_OBJ_ATTRIBUTE:
			s+=":=";
			break;
		case SET_TYPE_ATTRIBUTE:
			s+="::=";
			break;
    }
    s+="("+valueExpRef.toString()+") ";
    return s;
}

/*!	---|> [Object]	*/
Object * SetAttribute::execute(Runtime & rt) {
    ObjRef valueRef=NULL;
    if (!valueExpRef.isNull()) {
        valueRef=rt.executeObj(valueExpRef.get());
        if(!rt.assertNormalState(this))
			return NULL;

        /// Bug[20070703] fixed:
        if(!valueRef.isNull()){
            valueRef=valueRef->getRefOrCopy();
        }else{
            valueRef=Void::get();
        }
    }
    /// Local variable
    if (objRef.isNull()) {
        rt.assignToVariable(attrId,valueRef.get());
		return valueRef.detachAndDecrease();
    } /// obj.ident

	ObjRef obj2=rt.executeObj(objRef.get());
	if(!rt.assertNormalState(this))
		return NULL;

	if(obj2.isNull())
		obj2=Void::get();
	if(assignType == ASSIGN){
		if(!obj2->assignAttribute(attrId,valueRef.get())){
			rt.warn(std::string("Unkown attribute \"")+identifierIdToString(attrId)+"\" ("+
					(objRef.isNull()?"":objRef->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
			if(!obj2->setObjAttribute(attrId,valueRef.get())){
				rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
						(objRef.isNull()?"":objRef->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
			}
		}
	}else if(assignType == SET_OBJ_ATTRIBUTE){
		if(!obj2->setObjAttribute(attrId,valueRef.get()))
			rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
					(objRef.isNull()?"":objRef->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
	}else if(assignType == SET_TYPE_ATTRIBUTE){
		Type * t=obj2.toType<Type>();
		if(t){
			t->setTypeAttribute(attrId,valueRef.get());
		}else{
			rt.warn(std::string("Can not set typeAttr to non-Type-Object: \"")+identifierIdToString(attrId)+"\" ("+
					(objRef.isNull()?"":objRef->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")"
					+"Setting objAttr instead.");
			if(!obj2->setObjAttribute(attrId,valueRef.get())){
				rt.warn(std::string("Can't set object attribute \"")+identifierIdToString(attrId)+"\" ("+
						(objRef.isNull()?"":objRef->toDbgString())+"."+identifierIdToString(attrId)+"="+(valueRef.isNull()?"":valueRef->toDbgString())+")");
			}
		}
	}
	return valueRef.detachAndDecrease();
}
