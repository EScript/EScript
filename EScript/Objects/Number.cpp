#include "Number.h"
#include "../EScript.h"
#include "Bool.h"

#include <sstream>
#include <cmath>

using namespace EScript;

std::stack<Number *> Number::numberPool;
//---

//- Operators

/*!	[ESMF] - Number	*/
ESF(esmf_neg,0,0,Number::create( -caller->toDouble()))

/*!	[ESMF] Number + Number2	*/
ESF(esmf_plus,1,1,Number::create( caller->toDouble() + parameter[0]->toDouble()))

/*!	[ESMF] Number - Number2	*/
ESF(esmf_minus,1,1,Number::create( caller->toDouble()-parameter[0]->toDouble()))

/*!	[ESMF] Number * Number2	*/
ESF(esmf_mul,1,1,Number::create( caller->toDouble()*parameter[0]->toDouble()))

/*!	[ESMF] Number / Number2	*/
ES_FUNCTION(esmf_div) {
    assertParamCount(runtime,parameter.count(),1,1);
    double d=parameter[0]->toDouble();
    if (d==0){
        runtime.error("Division by zero");
        return NULL;
    }
    return  Number::create( caller->toDouble()/d);
}

/*!	[ESMF] Number & Number2	*/
ESF(esmf_and,1,1,Number::create( static_cast<double>(caller->toInt()&parameter[0]->toInt())))

/*!	[ESMF] Number | Number2	*/
ESF(esmf_or,1,1,Number::create( static_cast<double>(caller->toInt()|parameter[0]->toInt())))

/*!	[ESMF] Number ^ Number2	*/
ESF(esmf_not,1,1,Number::create( static_cast<double>(caller->toInt()^parameter[0]->toInt())))

/*!	[ESMF] Number % Number2	*/
ESF(esmf_mod,1,1,Number::create( static_cast<double>(caller->toInt()%parameter[0]->toInt())))

//- Modificators

/*!	[ESMF] Number++	*/
ES_FUNCTION(esmf_inc) {
    assertParamCount(runtime,parameter.count(),0,0);
    Number * n =assertType<Number>(runtime, caller);
    const double & val=n->getValue();
    Object * ret=Number::create(val);
    n->setValue(val+1.0);
    return ret;
}

/*!	[ESMF] caller ++Number	*/
ESF(esmf_incPre,0,0,((assertType<Number>(runtime, caller)->setValue(caller->toDouble()+1.0)),caller))

/*!	[ESMF] Number--	*/
ES_FUNCTION(esmf_dec) {
    assertParamCount(runtime,parameter.count(),0,0);
    Number * n =assertType<Number>(runtime, caller);
    const double & val=n->getValue();
    Object * ret=Number::create(val);
    n->setValue(val-1.0);
    return ret;
}

/*!	[ESMF] --Number	*/
ESF(esmf_decPre,0,0,(assertType<Number>(runtime, caller)->setValue(caller->toDouble()-1.0),caller))

/*!	[ESMF] Number += Number2	*/
ESF(esmf_plusAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toDouble()+parameter[0]->toDouble()),caller))

/*!	[ESMF] Numbern -= Number2	*/
ESF(esmf_minusAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toDouble()-parameter[0]->toDouble()),caller))

/*!	[ESMF] Numbern *= Number2	*/
ESF(esmf_mulAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toDouble()*parameter[0]->toDouble()),caller))

/*!	[ESMF] Numbern /= Number2	*/
ES_FUNCTION(esmf_divAssign) {
    assertParamCount(runtime,parameter.count(),1,1);
    Number * n =assertType<Number>(runtime, caller);
    double d=parameter[0]->toDouble();
    if (d==0){
        runtime.error("Division by zero");
        return NULL;
    }
    n->setValue(n->getValue()/d);
    return n;
}

/*!	[ESMF] Numbern %= Number2	*/
ESF(esmf_modAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toInt() % parameter[0].toInt()) ,caller))

/*!	[ESMF] Numbern |= Number2	*/
ESF(esmf_orAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toInt() | parameter[0].toInt()) ,caller))

/*!	[ESMF] Numbern &= Number2	*/
ESF(esmf_andAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toInt() & parameter[0].toInt()) ,caller))

/*!	[ESMF] Numbern ^= Number2	*/
ESF(esmf_notAssign,1,1,(assertType<Number>(runtime, caller)->setValue(caller->toInt() ^ parameter[0].toInt()) ,caller))


//- Comparisons

/*!	[ESMF] Numbern > Number2	*/
ESF(esmf_greater,1,1,Bool::create( caller->toDouble()>parameter[0]->toDouble()))

/*!	[ESMF] Numbern >= Number2	*/
ESF(esmf_greaterEqual,1,1,Bool::create( caller->toDouble()>=parameter[0]->toDouble()))

/*!	[ESMF] Numbern < Number2	*/
ESF(esmf_less,1,1,Bool::create( caller->toDouble()<parameter[0]->toDouble()))

/*!	[ESMF] Numbern <= Number2	*/
ESF(esmf_lessEqual,1,1,Bool::create( caller->toDouble()<=parameter[0]->toDouble()))

// ---

/*!	[ESMF] new Numbern([Number])	*/
ESF(esmf_constructor,0,1,Number::create( parameter[0].toDouble(0.0)))

/*!	[ESMF] Number Number.abs()	*/
ES_FUNCTION(esmf_abs) {
    assertParamCount(runtime,parameter.count(),0,0);
    double d=caller->toDouble();
    return  Number::create( d>0?d:-d);
}

/*!	[ESMF] Number Number.clamp(min,max)	*/
ES_FUNCTION(esmf_clamp) {
    assertParamCount(runtime,parameter.count(),2,2);
    double d=caller->toDouble();
    double min=parameter[0]->toDouble();
    if(d<=min)
		return Number::create(min);
	double max=parameter[1]->toDouble();
	return Number::create(d<=max ? d : max );
}

/*!	[ESMF] Number Number.round()	*/
ESF(esmf_round,0,0,Number::create(round( caller->toDouble())))

/*!	[ESMF] Number Number.floor()	*/
ESF(esmf_floor,0,0,Number::create(floor( caller->toDouble())))

/*!	[ESMF] Number Number.ceil()	*/
ESF(esmf_ceil,0,0,Number::create(ceil( caller->toDouble())))

/*!	[ESMF] Number Number.pow(Number)	*/
ESF(esmf_pow,1,1,Number::create(pow( caller->toDouble(),parameter[0]->toDouble())))

/*!	[ESMF] Number Number.sqrt	*/
ESF(esmf_sqrt,0,0,Number::create(sqrt( caller->toDouble())))

/*!	[ESMF] Number Number.sin	*/
ESF(esmf_sin,0,0,Number::create(sin(caller->toDouble())))

/*!	[ESMF] Number Number.cos	*/
ESF(esmf_cos,0,0,Number::create(cos(caller->toDouble())))

/*!	[ESMF] Number Number.acos	*/
ESF(esmf_acos,0,0,Number::create(acos(caller->toDouble())))

/*!	[ESMF] Number Number.asin	*/
ESF(esmf_asin,0,0,Number::create(asin(caller->toDouble())))

/*!	[ESMF] Number Number.atan	*/
ESF(esmf_atan,0,0,Number::create(atan(caller->toDouble())))

/*!	[ESMF] Number Number.log([basis=10])	*/
ESF(esmf_log,0,1,Number::create( parameter.count()>0?log(caller->toDouble())/log(parameter[0]->toDouble()) : log10(caller->toDouble())))

/*!	[ESMF] Number Number.ln()	*/
ESF(esmf_ln,0,0,Number::create(log(caller->toDouble())))

/*!	[ESMF] Number Number.tan	*/
ESF(esmf_tan,0,0,Number::create(tan( caller->toDouble())))

/*!	[ESMF] +1|-1 Number.sign()	*/
ESF(esmf_sign,0,0,Number::create( (caller->toFloat()<0)?-1.0:1.0))

/*!	[ESMF] String Number.toHex()	*/
ES_FUNCTION(esmf_toHex) {
    assertParamCount(runtime,parameter.count(),0,0);
    std::ostringstream sprinter;
    sprinter << std::hex << "0x"<<caller->toInt();
    return String::create(sprinter.str());
}

/*!	[ESMF] String Number.degToRad()	*/
ESF(esmf_degToRad,0,0,Number::create(caller->toFloat()*M_PI/180.0))

/*!	[ESMF] String Number.radToDeg()	*/
ESF(esmf_radToDeg,0,0,Number::create( (caller->toFloat()*180.0)/M_PI))

/*!	[ESMF] String Number.toIntStr()	*/
ES_FUNCTION(esmf_toIntStr) {
    assertParamCount(runtime,parameter.count(),0,0);
    std::ostringstream sprinter;
    sprinter <<caller->toInt();
    return String::create(sprinter.str());
}
/*!	[ESMF] String Number.format([Number precision=3[, Bool scientific=true[, Number width=0[, String fill='0']]]]) */
ESF(esmf_format,0,4,String::create(assertType<Number> (runtime, caller)->format(
		static_cast<std::streamsize >(parameter[0].toInt(3)), parameter[1].toBool(true),
		static_cast<std::streamsize >(parameter[2].toInt(0)), parameter[3].toString("0")[0])))

//---
Type* Number::typeObject=NULL;

/*!	initMembers	*/
void Number::init(EScript::Namespace & globals) {
//
    // Number ---|> [Object]
    typeObject=new Type(Object::getTypeObject());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

    declareConstant(&globals,getClassName(),typeObject);

    declareFunction(typeObject,"_constructor",esmf_constructor);
    declareFunction(typeObject,"+",esmf_plus);
    declareFunction(typeObject,"_-_pre",esmf_neg);
    declareFunction(typeObject,"-",esmf_minus);
    declareFunction(typeObject,"*",esmf_mul);
    declareFunction(typeObject,"/",esmf_div);
    declareFunction(typeObject,"&",esmf_and);
    declareFunction(typeObject,"|",esmf_or);
    declareFunction(typeObject,"^",esmf_not);
    declareFunction(typeObject,"%",esmf_mod);

    declareFunction(typeObject,"++_post",esmf_inc);
    declareFunction(typeObject,"++_pre",esmf_incPre);
    declareFunction(typeObject,"--_post",esmf_dec);
    declareFunction(typeObject,"--_pre",esmf_decPre);
    declareFunction(typeObject,"+=",esmf_plusAssign);
    declareFunction(typeObject,"-=",esmf_minusAssign);
    declareFunction(typeObject,"*=",esmf_mulAssign);
    declareFunction(typeObject,"/=",esmf_divAssign);
    declareFunction(typeObject,"%=",esmf_modAssign);
    declareFunction(typeObject,"|=",esmf_orAssign);
    declareFunction(typeObject,"&=",esmf_andAssign);
    declareFunction(typeObject,"^=",esmf_notAssign);

    declareFunction(typeObject,">",esmf_greater);
    declareFunction(typeObject,">=",esmf_greaterEqual);
    declareFunction(typeObject,"<",esmf_less);
    declareFunction(typeObject,"<=",esmf_lessEqual);
//    declareFunction(typeObject,"==",esmf_equal);
//    declareFunction(typeObject,"!=",esmf_notEqual);
//    declareFunction(typeObject,"===",esmf_identical);
//    declareFunction(typeObject,"!==",esmf_notIdentical);
    declareFunction(typeObject,"clamp",esmf_clamp);
    declareFunction(typeObject,"abs",esmf_abs);
    declareFunction(typeObject,"round",esmf_round);
    declareFunction(typeObject,"floor",esmf_floor);
    declareFunction(typeObject,"ceil",esmf_ceil);
    declareFunction(typeObject,"pow",esmf_pow);
    declareFunction(typeObject,"sqrt",esmf_sqrt);
    declareFunction(typeObject,"sin",esmf_sin);
    declareFunction(typeObject,"cos",esmf_cos);
    declareFunction(typeObject,"tan",esmf_tan);
    declareFunction(typeObject,"log",esmf_log);
    declareFunction(typeObject,"ln",esmf_ln);

    declareFunction(typeObject,"acos",esmf_acos);
    declareFunction(typeObject,"asin",esmf_asin);
    declareFunction(typeObject,"atan",esmf_atan);
    declareFunction(typeObject,"sign",esmf_sign);


    declareFunction(typeObject,"degToRad",esmf_degToRad);
    declareFunction(typeObject,"radToDeg",esmf_radToDeg);
    //TODO: sin,cos,tan,sqrt,exp,log,...

    declareFunction(typeObject,"toHex",esmf_toHex);
    declareFunction(typeObject,"toIntStr",esmf_toIntStr);
    declareFunction(typeObject,"format",esmf_format);
//
}

//---

/*! (static) */
Number * Number::create(double value){
    #ifdef ES_DEBUG_MEMORY
    return new Number(value);
    #endif
    if(numberPool.empty()){
        return new Number(value);
    }else{
        Number * n=numberPool.top();
        numberPool.pop();
        n->setValue(value);
        return n;
    }
}

/*! (static) */
Number * Number::create(double value,Type * type){
    #ifdef ES_DEBUG_MEMORY
    return new Number(value,type);
    #endif

    if(type==Number::typeObject)
        return create(value);
    else
        return new Number(value,type);
}

/*! (static) */
void Number::release(Number * n){
    #ifdef ES_DEBUG_MEMORY
    delete n;
    return;
    #endif
    if(n->getType()!=typeObject){
        delete n;
        std::cout << "Found diff NumberType\n";
    }else{
        numberPool.push(n);
    }
}
//---

/*!	(ctor)	*/
Number::Number(double _value,Type * type,bool isReference):
    Object(type?type:typeObject),valuePtr(NULL){
    if(!isReference)
        doubleValue=_value;
    //ctor
}

/*!	(dtor)	*/
Number::~Number() {
    //dtor
}

/*!	---o	*/
double Number::getValue()const{
    return doubleValue;//*((double *)value);
}

/*!	---o	*/
void Number::setValue(double _value){
//    *((double *)value)=_value;
    doubleValue=_value;
}

/*!	---|> [Object]	*/
Object * Number::clone() const {
    return Number::create(getValue(),getType());
}

//!
std::string Number::format(std::streamsize precision, bool scientific, std::streamsize width, char fill) const {
	std::ostringstream sprinter;
	sprinter.precision(precision);
	if (scientific) {
		sprinter.setf(std::ios::scientific, std::ios::floatfield);
	} else {
		sprinter.setf(std::ios::fixed, std::ios::floatfield);
	}
	sprinter.fill(fill);
	sprinter.width(width);
	sprinter << getValue();
	return sprinter.str();
}

/*!	---|> [Object]	*/
std::string Number::toString()const {
    std::ostringstream sprinter;
    sprinter << getValue();
    return sprinter.str();
}

/*!	---|> [Object]	*/
double Number::toDouble()const {
    return getValue();
}

/*!	---|> [Object]	*/
bool Number::toBool()const {
    return true;//getValue()!=0;
}

/*!	---|> [Object]	*/
bool Number::rt_isEqual(Runtime & ,const ObjPtr o){
    return getValue()==o.toDouble();
}
