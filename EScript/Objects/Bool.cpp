#include "Bool.h"

#include "../EScript.h"

#include "Function.h"
#include "../Runtime/Runtime.h"

#include <sstream>
#include <math.h>

namespace EScript{

namespace EMembers{

/*! \name EClass: Bool ---|> Object
    \see EScript::Bool
*/
//@{

//- Operators

/*!	[ESMF] Bool Bool + ((Bool)obj)	*/
ESF(esmf_Bool_plus,1,1,Bool::create( caller->toBool() + parameter[0]->toBool()))

/*!	Bool Bool - ((Bool)obj)	*/
ESF(esmf_Bool_minus,1,1,Bool::create( caller->toBool()-parameter[0]->toBool()))

/*!	Bool Bool & ((Bool)obj)	*/
ESF(esmf_Bool_and,1,1,Bool::create( caller->toBool()&parameter[0]->toBool()))

/*!	[ESMF] Bool Bool | ((Bool)obj)	*/
ESF(esmf_Bool_or,1,1,Bool::create( caller->toBool()|parameter[0]->toBool()))

/*!	[ESMF] Bool Bool ^ ((Bool)obj)	*/
ESF(esmf_Bool_not,1,1,Bool::create( caller->toBool()^parameter[0]->toBool()))

/*!	[ESMF] Bool !Bool	*/
ESF(esmf_Bool_neg,0,0,Bool::create(! caller->toBool()))

/*!	[ESMF] Bool |= Bool	*/
ESF(esmf_Bool_orAssign,1,1,(assertType<Bool>(runtime, caller)->setValue(caller->toBool() | parameter[0].toBool()) ,caller))

/*!	[ESMF] Bool &= Bool	*/
ESF(esmf_Bool_andAssign,1,1,(assertType<Bool>(runtime, caller)->setValue(caller->toBool() & parameter[0].toBool()) ,caller))


//- Comparisons

/*!	[ESMF] Bool Bool > ((Bool)obj)	*/
ESF(esmf_Bool_greater,1,1,Bool::create( caller->toBool()>parameter[0]->toBool()))

/*!	[ESMF] Bool Bool < ((Bool)obj)	*/
ESF(esmf_Bool_less,1,1,Bool::create( caller->toBool()<parameter[0]->toBool()))

}// namespace EMembers
//---
Type* Bool::typeObject=NULL;

/*!	initMembers	*/
void Bool::init(EScript::Namespace & globals) {
//
    // Bool ---|> [Object]
    typeObject=new Type(Object::getTypeObject());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);
    declareConstant(&globals,getClassName(),typeObject);

    using namespace EMembers;

    declareFunction(typeObject,"+",esmf_Bool_plus);
    declareFunction(typeObject,"-",esmf_Bool_minus);
    declareFunction(typeObject,"&",esmf_Bool_and);
    declareFunction(typeObject,"|",esmf_Bool_or);
    declareFunction(typeObject,"^",esmf_Bool_not);
    declareFunction(typeObject,"!_pre",esmf_Bool_neg);
    declareFunction(typeObject,"|=",esmf_Bool_orAssign);
    declareFunction(typeObject,"&=",esmf_Bool_andAssign);


    declareFunction(typeObject,">",esmf_Bool_greater);
    declareFunction(typeObject,"<",esmf_Bool_less);
    // ,FLAG_CALL_BY_VALUE
}
//----
std::stack<Bool *> boolPool=std::stack<Bool *>();

Bool * Bool::create(bool value){
    #ifdef ES_DEBUG_MEMORY
    return new Bool(value);
    #endif
    if(boolPool.empty()){
        for(int i=0;i<32;i++){
            boolPool.push(new Bool(false));
        }
        return create(value);
    }else{
        Bool * o=boolPool.top();
        boolPool.pop();
        o->value=value;
//        std::cout << ".";
        return o;
    }

}
void Bool::release(Bool * o){
    #ifdef ES_DEBUG_MEMORY
    delete o;
    return;
    #endif
    if(o->getType()!=typeObject){
        delete o;
        std::cout << "Found diff BoolType\n";
    }else{
        boolPool.push(o);
    }
}

//---

Bool::Bool(bool _value,Type * type):
        Object(type?type:typeObject),value(_value) {
    //ctor
}

Bool::~Bool() {
    //dtor
}

/*!	---|> [Object]	*/
Object * Bool::clone() const {
    return Bool::create(value);
}

/*!	---|> [Object]	*/
std::string Bool::toString()const {
    return value?"true":"false";
}

/*!	---|> [Object]	*/
double Bool::toDouble()const {
    return value?1:0;
}

/*!	---|> [Object]	*/
bool Bool::toBool()const {
    return value;
}

/*!	---|> [Object]	*/
bool Bool::rt_isEqual(Runtime &,const ObjPtr o){
    return  value==o.toBool(false);
}
} // namespace EScript
