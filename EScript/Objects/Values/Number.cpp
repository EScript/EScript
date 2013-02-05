// Number.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Number.h"
#include "../../EScript.h"
#include "Bool.h"

#include <cmath>
#include <sstream>
#include <stack>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#endif

namespace EScript{

template<>
double convertTo<double>(Runtime &runtime,ObjPtr src){
	if(src.isNotNull()){
		if(src->_getInternalTypeId()==_TypeIds::TYPE_NUMBER){
			return **static_cast<Number*>(src.get());
		}else if(src->_getInternalTypeId()==_TypeIds::TYPE_STRING){
			return src->toDouble();
		}else{
			runtime.warn("Converting "+  src.toDbgString() +" to Number.");
//			assertType_throwError(runtime,src,Number::getClassName());
			return src->toDouble();
		}
	}
	return 0.0;
//	return src.toDouble();
}


//! initMembers
void Number::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] new Numbern([Number])
	ESF_DECLARE(typeObject,"_constructor",0,1,Number::create(parameter[0].toDouble(0.0)))

	//- Operators

	//! [ESMF] + Number
	ESF_DECLARE(typeObject,"+_pre",0,0,caller)

	//! [ESMF] - Number
	ESF_DECLARE(typeObject,"_-_pre",0,0,-caller->toDouble())

	//! [ESMF] Number + Number2
	ESF_DECLARE(typeObject,"+",1,1, caller->toDouble() + parameter[0].to<double>(runtime))

	//! [ESMF] Number - Number2
	ESF_DECLARE(typeObject,"-",1,1,caller->toDouble()-parameter[0].to<double>(runtime))

	//! [ESMF] Number * Number2
	ESF_DECLARE(typeObject,"*",1,1,caller->toDouble()*parameter[0].to<double>(runtime))

	//! [ESMF] Number / Number2
	ES_FUNCTION_DECLARE(typeObject,"/",1,1,{
		const double d = parameter[0].to<double>(runtime);
		if(d==0){
			runtime.setException("Division by zero");
			return nullptr;
		}
		return caller->toDouble()/d;
	})

	//! [ESMF] Number & Number2
	ESF_DECLARE(typeObject,"&",1,1, caller->toInt()&parameter[0].to<int>(runtime))

	//! [ESMF] Number | Number2
	ESF_DECLARE(typeObject,"|",1,1, caller->toInt()|parameter[0].to<int>(runtime))

	//! [ESMF] Number ^ Number2
	ESF_DECLARE(typeObject,"^",1,1, caller->toInt()^parameter[0].to<int>(runtime))

	//! [ESMF] Number % Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"%",1,1,{
		const double d = parameter[0].to<double>(runtime);
		if(d==0){
			runtime.setException("Modulo with zero");
			return nullptr;
		}
		return self->modulo(d);
	})

	//- Modificators

	//! [ESMF] Number++
	ES_MFUNCTION_DECLARE(typeObject,Number,"++_post",0,0,{
		const double val = self->getValue();
		self->setValue(val+1.0);
		return val;
	})

	//! [ESMF] Number--
	ES_MFUNCTION_DECLARE(typeObject,Number,"--_post",0,0,{
		const double val = self->getValue();
		self->setValue(val-1.0);
		return val;
	})

	//! [ESMF] caller ++Number
	ESMF_DECLARE(typeObject,Number,"++_pre",0,0,((self->setValue(caller->toDouble()+1.0)),caller))

	//! [ESMF] --Number
	ESMF_DECLARE(typeObject,Number,"--_pre",0,0,(self->setValue(caller->toDouble()-1.0),caller))

	//! [ESMF] Number += Number2
	ESMF_DECLARE(typeObject,Number,"+=",1,1,(self->setValue(caller->toDouble()+parameter[0].to<double>(runtime)),caller))

	//! [ESMF] Numbern -= Number2
	ESMF_DECLARE(typeObject,Number,"-=",1,1,(self->setValue(caller->toDouble()-parameter[0].to<double>(runtime)),caller))

	//! [ESMF] Numbern *= Number2
	ESMF_DECLARE(typeObject,Number,"*=",1,1,(self->setValue(caller->toDouble()*parameter[0].to<double>(runtime)),caller))

	//! [ESMF] Numbern /= Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"/=",1,1,{
		const double d = parameter[0].to<double>(runtime);
		if(d==0){
			runtime.setException("Division by zero");
			return nullptr;
		}
		self->setValue(self->getValue()/d);
		return self;
	})

	//! [ESMF] Numbern %= Number2
	ES_MFUNCTION_DECLARE(typeObject,Number,"%=",1,1,{
		const double d = parameter[0].to<double>(runtime);
		if(d==0){
			runtime.setException("Modulo with zero");
			return nullptr;
		}
		self->setValue(self->modulo(d));
		return self;
	})
	ESMF_DECLARE(typeObject,Number,"%=",1,1,(self->setValue(caller->toInt() % parameter[0].to<int>(runtime)) ,caller))

	//! [ESMF] Numbern |= Number2
	ESMF_DECLARE(typeObject,Number,"|=",1,1,(self->setValue(caller->toInt() | parameter[0].to<int>(runtime)) ,caller))

	//! [ESMF] Numbern &= Number2
	ESMF_DECLARE(typeObject,Number,"&=",1,1,(self->setValue(caller->toInt() & parameter[0].to<int>(runtime)) ,caller))

	//! [ESMF] Numbern ^= Number2
	ESMF_DECLARE(typeObject,Number,"^=",1,1,(self->setValue(caller->toInt() ^ parameter[0].to<int>(runtime)) ,caller))


// ~=

	//- Comparisons

	//! [ESMF] Numbern > Number2
	ESF_DECLARE(typeObject,">",1,1,caller->toDouble()>parameter[0].to<double>(runtime))

	//! [ESMF] Numbern >= Number2
	ESF_DECLARE(typeObject,">=",1,1,caller->toDouble()>=parameter[0].to<double>(runtime))

	//! [ESMF] Numbern < Number2
	ESF_DECLARE(typeObject,"<",1,1,caller->toDouble()<parameter[0].to<double>(runtime))

	//! [ESMF] Numbern <= Number2
	ESF_DECLARE(typeObject,"<=",1,1,caller->toDouble()<=parameter[0].to<double>(runtime))

	//! [ESMF] Bool (Numbern ~= Number2)
	ESF_DECLARE(typeObject,"~=",1,1, Number::matches(caller->toFloat(), parameter[0].to<float>(runtime)))

	// - Misc

	//! [ESMF] Number Number.abs()
	ES_FUNCTION_DECLARE(typeObject,"abs",0,0,{
		assertParamCount(runtime,parameter.count(),0,0);
		const double d = caller->toDouble();
		return  d>0?d:-d;
	})

	//! [ESMF] Number Number.acos
	ESF_DECLARE(typeObject,"acos",0,0,std::acos(caller->toDouble()))

	//! [ESMF] Number Number.asin
	ESF_DECLARE(typeObject,"asin",0,0,std::asin(caller->toDouble()))

	//! [ESMF] Number Number.atan
	ESF_DECLARE(typeObject,"atan",0,0,std::atan(caller->toDouble()))

	//! [ESMF] Number Number.ceil()
	ESF_DECLARE(typeObject,"ceil",0,0,std::ceil( caller->toDouble()))

	//! [ESMF] Number Number.cos
	ESF_DECLARE(typeObject,"cos",0,0,std::cos(caller->toDouble()))

	//! [ESMF] Number Number.clamp(min,max)
	ES_FUNCTION_DECLARE(typeObject,"clamp",2,2, {
		const double d = caller->toDouble();
		const double min = parameter[0].to<double>(runtime);
		if(d<=min)
			return min;
		const double max = parameter[1].to<double>(runtime);
		return d<=max ? d : max ;
	})

	//! [ESMF] String Number.degToRad()
	ESF_DECLARE(typeObject,"degToRad",0,0,(caller->toFloat()*M_PI/180.0))

	//! [ESMF] Number Number.floor()
	ESF_DECLARE(typeObject,"floor",0,0,(std::floor( caller->toDouble())))

	//! [ESMF] String Number.format([Number precision = 3[, Bool scientific = true[, Number width = 0[, String fill='0']]]])
	ESMF_DECLARE(typeObject,Number,"format",0,4,self->format(
			static_cast<std::streamsize >(parameter[0].to<int>(runtime,3)), parameter[1].toBool(true),
			static_cast<std::streamsize >(parameter[2].to<int>(runtime,0)), parameter[3].toString("0")[0]))

	//! [ESMF] Number Number.ln()
	ESF_DECLARE(typeObject,"ln",0,0,std::log(caller->toDouble()))

	//! [ESMF] Number Number.log([basis = 10])
	ESF_DECLARE(typeObject,"log",0,1,( parameter.count()>0?log(caller->toDouble())/log(parameter[0].to<double>(runtime)) : log10(caller->toDouble())))

	//! [ESMF] bool Number.matches(other)
	ESF_DECLARE(typeObject,"matches",1,1, Number::matches(caller->toFloat(), parameter[0].to<float>(runtime)))

	//! [ESMF] Number Number.pow(Number)
	ESF_DECLARE(typeObject,"pow",1,1,std::pow( caller->toDouble(),parameter[0].to<double>(runtime)))

	//! [ESMF] String Number.radToDeg()
	ESF_DECLARE(typeObject,"radToDeg",0,0,(caller->toDouble()*180.0)/M_PI)

	/*! [ESMF] Number Number.round( [reference = 1.0] )
		@param reference Reference value to which should be rounded:  x.round(reference) ^== reference * round(x/reference)
		@example (123.456).round(0.1) == 123.5
				(123.456).round(5) == 125
				(123.456).round(10) == 120 */
	ES_FUNCTION_DECLARE(typeObject,"round",0,1,{
		if(parameter.count()==0)
			return std::round( caller->toDouble());
		const double reference = parameter[0].to<double>(runtime);
		if(reference==0){
			runtime.setException("round with zero");
			return nullptr;
		}
		return std::round(caller->toDouble()/reference) * reference;
	})

	//! [ESMF] +1|-1 Number.sign()
	ESF_DECLARE(typeObject,"sign",0,0,(caller->toFloat()<0)?-1.0:1.0)

	//! [ESMF] Number Number.sin
	ESF_DECLARE(typeObject,"sin",0,0,std::sin(caller->toDouble()))

	//! [ESMF] Number Number.sqrt
	ESF_DECLARE(typeObject,"sqrt",0,0,std::sqrt( caller->toDouble()))

	//! [ESMF] Number Number.tan
	ESF_DECLARE(typeObject,"tan",0,0,std::tan( caller->toDouble()))

	//! [ESMF] String Number.toHex()
	ES_FUNCTION_DECLARE(typeObject,"toHex",0,0,{
		std::ostringstream sprinter;
		sprinter << std::hex << "0x"<<caller->toInt();
		return sprinter.str();
	})

	//! [ESMF] String Number.toIntStr()
	ES_FUNCTION_DECLARE(typeObject,"toIntStr",0,0,{
		std::ostringstream sprinter;
		sprinter <<caller->toInt();
		return sprinter.str();
	})
}

//------------------------------------------------------
static std::stack<Number *> pool;

//! (static)
Number * Number::create(double value){
//static int count = 0;
//std::cout << ++count<<" "; //6987 (5933)

	#ifdef ES_DEBUG_MEMORY
	return new Number(value);
	#endif
	if(pool.empty()){
		return new Number(value);
	}else{
		Number * n = pool.top();
		pool.pop();
		n->setValue(value);
		return n;
	}
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
		pool.push(n);
	}
}
//----------------------------------------------------------

//! (ctor)
Number::Number(double _value) : Object(getTypeObject()),value(_value){
	//ctor
}

double Number::modulo(double m)const{
	const double a = getValue();
	const double a_m = a/m;
	return a - (a_m<0 ? ceil(a_m) : floor(a_m)) * m;
}

//!
std::string Number::format(std::streamsize precision, bool scientific, std::streamsize width, char fill) const {
	std::ostringstream sprinter;
	sprinter.precision(precision);
	if(scientific) {
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
bool Number::rt_isEqual(Runtime & ,const ObjPtr & o){
	return getValue()==o.toDouble();
}
}
