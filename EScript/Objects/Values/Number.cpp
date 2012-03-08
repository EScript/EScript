// Number.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Number.h"
#include "../../EScript.h"
#include "Bool.h"

#include <sstream>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#endif

#if defined(_MSC_VER)
static inline double round(const double & x) { return floor(x + 0.5); }
static inline float roundf(const float & x) { return floorf(x + 0.5f); }
#endif

using namespace EScript;

std::stack<Number *> Number::numberPool;

//! (static)
Type * Number::getTypeObject(){
	// [Number] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}

//! initMembers
void Number::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] new Numbern([Number])
	ESF_DECLARE(typeObject,"_constructor",0,1,Number::create( parameter[0].toDouble(0.0)))

	//- Operators

	//! [ESMF] - Number
	ESF_DECLARE(typeObject,"_-_pre",0,0,Number::create( -caller->toDouble()))

	//! [ESMF] Number + Number2
	ESF_DECLARE(typeObject,"+",1,1,Number::create( caller->toDouble() + parameter[0]->toDouble()))

	//! [ESMF] Number - Number2
	ESF_DECLARE(typeObject,"-",1,1,Number::create( caller->toDouble()-parameter[0]->toDouble()))

	//! [ESMF] Number * Number2
	ESF_DECLARE(typeObject,"*",1,1,Number::create( caller->toDouble()*parameter[0]->toDouble()))

	//! [ESMF] Number / Number2
	ES_FUNCTION_DECLARE(typeObject,"/",1,1,{
		const double d=parameter[0]->toDouble();
		if (d==0){
			runtime.setException("Division by zero");
			return NULL;
		}
		return  Number::create( caller->toDouble()/d);
	})

	//! [ESMF] Number & Number2
	ESF_DECLARE(typeObject,"&",1,1,Number::create( static_cast<double>(caller->toInt()&parameter[0]->toInt())))

	//! [ESMF] Number | Number2
	ESF_DECLARE(typeObject,"|",1,1,Number::create( static_cast<double>(caller->toInt()|parameter[0]->toInt())))

	//! [ESMF] Number ^ Number2
	ESF_DECLARE(typeObject,"^",1,1,Number::create( static_cast<double>(caller->toInt()^parameter[0]->toInt())))

	//! [ESMF] Number % Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"%",1,1,{
		const double d=parameter[0]->toDouble();
		if (d==0){
			runtime.setException("Modulo with zero");
			return NULL;
		}
		return  Number::create( self->modulo(d) );
	})

	//- Modificators

	//! [ESMF] Number++
	ES_MFUNCTION_DECLARE(typeObject,Number,"++_post",0,0,{
		const double & val=self->getValue();
		Object * ret=Number::create(val);
		self->setValue(val+1.0);
		return ret;
	})

	//! [ESMF] Number--
	ES_MFUNCTION_DECLARE(typeObject,Number,"--_post",0,0,{
		const double & val=self->getValue();
		Object * ret=Number::create(val);
		self->setValue(val-1.0);
		return ret;
	})

	//! [ESMF] caller ++Number
	ESMF_DECLARE(typeObject,Number,"++_pre",0,0,((self->setValue(caller->toDouble()+1.0)),caller))

	//! [ESMF] --Number
	ESMF_DECLARE(typeObject,Number,"--_pre",0,0,(self->setValue(caller->toDouble()-1.0),caller))

	//! [ESMF] Number += Number2
	ESMF_DECLARE(typeObject,Number,"+=",1,1,(self->setValue(caller->toDouble()+parameter[0]->toDouble()),caller))

	//! [ESMF] Numbern -= Number2
	ESMF_DECLARE(typeObject,Number,"-=",1,1,(self->setValue(caller->toDouble()-parameter[0]->toDouble()),caller))

	//! [ESMF] Numbern *= Number2
	ESMF_DECLARE(typeObject,Number,"*=",1,1,(self->setValue(caller->toDouble()*parameter[0]->toDouble()),caller))

	//! [ESMF] Numbern /= Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"/=",1,1,{
		const double d=parameter[0]->toDouble();
		if (d==0){
			runtime.setException("Division by zero");
			return NULL;
		}
		self->setValue(self->getValue()/d);
		return self;
	})

	//! [ESMF] Numbern %= Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"%=",1,1,{
		const double d=parameter[0]->toDouble();
		if (d==0){
			runtime.setException("Modulo with zero");
			return NULL;
		}
		self->setValue(self->modulo(d));
		return self;
	})
	ESMF_DECLARE(typeObject,Number,"%=",1,1,(self->setValue(caller->toInt() % parameter[0].toInt()) ,caller))

	//! [ESMF] Numbern |= Number2
	ESMF_DECLARE(typeObject,Number,"|=",1,1,(self->setValue(caller->toInt() | parameter[0].toInt()) ,caller))

	//! [ESMF] Numbern &= Number2
	ESMF_DECLARE(typeObject,Number,"&=",1,1,(self->setValue(caller->toInt() & parameter[0].toInt()) ,caller))

	//! [ESMF] Numbern ^= Number2
	ESMF_DECLARE(typeObject,Number,"^=",1,1,(self->setValue(caller->toInt() ^ parameter[0].toInt()) ,caller))


// ~=

	//- Comparisons

	//! [ESMF] Numbern > Number2
	ESF_DECLARE(typeObject,">",1,1,Bool::create( caller->toDouble()>parameter[0]->toDouble()))

	//! [ESMF] Numbern >= Number2
	ESF_DECLARE(typeObject,">=",1,1,Bool::create( caller->toDouble()>=parameter[0]->toDouble()))

	//! [ESMF] Numbern < Number2
	ESF_DECLARE(typeObject,"<",1,1,Bool::create( caller->toDouble()<parameter[0]->toDouble()))

	//! [ESMF] Numbern <= Number2
	ESF_DECLARE(typeObject,"<=",1,1,Bool::create( caller->toDouble()<=parameter[0]->toDouble()))

	//! [ESMF] Bool (Numbern ~= Number2)
	ESF_DECLARE(typeObject,"~=",1,1,Bool::create( Number::matches(caller->toFloat(), parameter[0]->toFloat())))

	// - Misc

	//! [ESMF] Number Number.abs()
	ES_FUNCTION_DECLARE(typeObject,"abs",0,0,{
		assertParamCount(runtime,parameter.count(),0,0);
		const double d=caller->toDouble();
		return  Number::create( d>0?d:-d);
	})

	//! [ESMF] Number Number.acos
	ESF_DECLARE(typeObject,"acos",0,0,Number::create(acos(caller->toDouble())))

	//! [ESMF] Number Number.asin
	ESF_DECLARE(typeObject,"asin",0,0,Number::create(asin(caller->toDouble())))

	//! [ESMF] Number Number.atan
	ESF_DECLARE(typeObject,"atan",0,0,Number::create(atan(caller->toDouble())))

	//! [ESMF] Number Number.ceil()
	ESF_DECLARE(typeObject,"ceil",0,0,Number::create(ceil( caller->toDouble())))

	//! [ESMF] Number Number.cos
	ESF_DECLARE(typeObject,"cos",0,0,Number::create(cos(caller->toDouble())))

	/*!	[ESMF] Number Number.clamp(min,max)	*/
	ES_FUNCTION_DECLARE(typeObject,"clamp",2,2, {
		const double d=caller->toDouble();
		const double min=parameter[0]->toDouble();
		if(d<=min)
			return Number::create(min);
		const double max=parameter[1]->toDouble();
		return Number::create(d<=max ? d : max );
	})

	//! [ESMF] String Number.degToRad()
	ESF_DECLARE(typeObject,"degToRad",0,0,Number::create(caller->toFloat()*M_PI/180.0))

	//! [ESMF] Number Number.floor()
	ESF_DECLARE(typeObject,"floor",0,0,Number::create(floor( caller->toDouble())))

	//! [ESMF] String Number.format([Number precision=3[, Bool scientific=true[, Number width=0[, String fill='0']]]])
	ESMF_DECLARE(typeObject,Number,"format",0,4,String::create(self->format(
			static_cast<std::streamsize >(parameter[0].toInt(3)), parameter[1].toBool(true),
			static_cast<std::streamsize >(parameter[2].toInt(0)), parameter[3].toString("0")[0])))

	//! [ESMF] Number Number.ln()
	ESF_DECLARE(typeObject,"ln",0,0,Number::create(log(caller->toDouble())))

	//! [ESMF] Number Number.log([basis=10])
	ESF_DECLARE(typeObject,"log",0,1,Number::create( parameter.count()>0?log(caller->toDouble())/log(parameter[0]->toDouble()) : log10(caller->toDouble())))

	//! [ESMF] bool Number.matches(other)
	ESF_DECLARE(typeObject,"matches",1,1,Bool::create( Number::matches(caller->toFloat(), parameter[0]->toFloat())))

	//! [ESMF] Number Number.pow(Number)
	ESF_DECLARE(typeObject,"pow",1,1,Number::create(pow( caller->toDouble(),parameter[0]->toDouble())))

	//! [ESMF] String Number.radToDeg()
	ESF_DECLARE(typeObject,"radToDeg",0,0,Number::create( (caller->toFloat()*180.0)/M_PI))

	/*! [ESMF] Number Number.round( [reference=1.0] )
		@param reference Reference value to which should be rounded:  x.round(reference) ^== reference * round(x/reference)
		@example (123.456).round(0.1) == 123.5
				(123.456).round(5) == 125
				(123.456).round(10) == 120 */
	ES_FUNCTION_DECLARE(typeObject,"round",0,1,{
		if(parameter.count()==0)
			return 	Number::create(round( caller->toDouble()));
		const double reference = parameter[0].toDouble();
		if (reference==0){
			runtime.setException("round with zero");
			return NULL;
		}
		return Number::create(round(caller->toDouble()/reference) * reference);
	})

	//! [ESMF] +1|-1 Number.sign()
	ESF_DECLARE(typeObject,"sign",0,0,Number::create( (caller->toFloat()<0)?-1.0:1.0))

	//! [ESMF] Number Number.sin
	ESF_DECLARE(typeObject,"sin",0,0,Number::create(sin(caller->toDouble())))

	//! [ESMF] Number Number.sqrt
	ESF_DECLARE(typeObject,"sqrt",0,0,Number::create(sqrt( caller->toDouble())))

	//! [ESMF] Number Number.tan
	ESF_DECLARE(typeObject,"tan",0,0,Number::create(tan( caller->toDouble())))

	//! [ESMF] String Number.toHex()
	ES_FUNCTION_DECLARE(typeObject,"toHex",0,0,{
		std::ostringstream sprinter;
		sprinter << std::hex << "0x"<<caller->toInt();
		return String::create(sprinter.str());
	})

	//! [ESMF] String Number.toIntStr()
	ES_FUNCTION_DECLARE(typeObject,"toIntStr",0,0,{
		std::ostringstream sprinter;
		sprinter <<caller->toInt();
		return String::create(sprinter.str());
	})
}

//------------------------------------------------------

//! (static)
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

//! (static)
Number * Number::create(double value,Type * type){
	#ifdef ES_DEBUG_MEMORY
	return new Number(value,type);
	#endif

	if(type==Number::getTypeObject())
		return create(value);
	else
		return new Number(value,type);
}

//! (static)
void Number::release(Number * n){
	#ifdef ES_DEBUG_MEMORY
	delete n;
	return;
	#endif
	if(n->getType()!=getTypeObject()){
		delete n;
		std::cout << "Found diff NumberType\n";
	}else{
		numberPool.push(n);
	}
}
//----------------------------------------------------------

//! (ctor)
Number::Number(double _value,Type * type,bool isReference):
	Object(type?type:getTypeObject()),valuePtr(NULL){
	if(!isReference)
		doubleValue=_value;
	//ctor
}

//! (dtor)
Number::~Number() {
	//dtor
}

//! ---o
double Number::getValue()const{
	return doubleValue;//*((double *)value);
}

//! ---o
void Number::setValue(double _value){
//    *((double *)value)=_value;
	doubleValue=_value;
}

double Number::modulo(double m)const{
	const double a = getValue();
	const double a_m = a/m;
	return a - (a_m<0 ? ceil(a_m) : floor(a_m)) * m;
}

//! ---|> [Object]
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

//! ---|> [Object]
std::string Number::toString()const {
	std::ostringstream sprinter;
	sprinter << getValue();
	return sprinter.str();
}

//! ---|> [Object]
double Number::toDouble()const {
	return getValue();
}

//! ---|> [Object]
bool Number::toBool()const {
	return true;//getValue()!=0;
}

//! ---|> [Object]
bool Number::rt_isEqual(Runtime & ,const ObjPtr o){
	return getValue()==o.toDouble();
}
//! ---|> Object
void Number::_asmOut(std::ostream & out){
	out<<"push (Number) "<<toDouble()<<"\n";
}
