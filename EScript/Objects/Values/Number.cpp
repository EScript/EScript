// Number.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Number.h"
#include "../../Basics.h"

#include <cmath>
#include <sstream>
#include <stack>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#endif

namespace EScript{

//! (static)
Type * Number::getTypeObject(){
	static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
	return typeObject;
}

//! initMembers
void Number::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());
	typeObject->setFlag(Type::FLAG_CALL_BY_VALUE,true);

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] new Numbern([Number])
	ES_CTOR(typeObject,0,1,Number::create(parameter[0].toDouble(0.0)))

	//- Operators

	//! [ESMF] + Number
	ES_FUN(typeObject,"+_pre",0,0,thisEObj)

	//! [ESMF] - Number
	ES_FUN(typeObject,"_-_pre",0,0,-thisEObj->toDouble())

	//! [ESMF] Number + Number2
	ES_FUN(typeObject,"+",1,1, thisEObj->toDouble() + parameter[0].to<double>(rt))

	//! [ESMF] Number - Number2
	ES_FUN(typeObject,"-",1,1,thisEObj->toDouble()-parameter[0].to<double>(rt))

	//! [ESMF] Number * Number2
	ES_FUN(typeObject,"*",1,1,thisEObj->toDouble()*parameter[0].to<double>(rt))

	//! [ESMF] Number / Number2
	ES_FUNCTION2(typeObject,"/",1,1,{
		const double d = parameter[0].to<double>(rt);
		if(d==0){
			rt.setException("Division by zero");
			return nullptr;
		}
		return thisEObj->toDouble()/d;
	})

	//! [ESMF] Number & Number2
	ES_FUN(typeObject,"&",1,1, thisEObj->toInt()&parameter[0].to<int>(rt))

	//! [ESMF] Number | Number2
	ES_FUN(typeObject,"|",1,1, thisEObj->toInt()|parameter[0].to<int>(rt))

	//! [ESMF] Number ^ Number2
	ES_FUN(typeObject,"^",1,1, thisEObj->toInt()^parameter[0].to<int>(rt))

	//! [ESMF] Number % Number2
	ES_MFUNCTION(typeObject,Number,"%",1,1,{
		const double d = parameter[0].to<double>(rt);
		if(d==0){
			rt.setException("Modulo with zero");
			return nullptr;
		}
		return thisObj->modulo(d);
	})

	//- Modificators

	//! [ESMF] Number++
	ES_MFUNCTION(typeObject,Number,"++_post",0,0,{
		const double val = thisObj->getValue();
		thisObj->setValue(val+1.0);
		return val;
	})

	//! [ESMF] Number--
	ES_MFUNCTION(typeObject,Number,"--_post",0,0,{
		const double val = thisObj->getValue();
		thisObj->setValue(val-1.0);
		return val;
	})

	//! [ESMF] caller ++Number
	ES_MFUN(typeObject,Number,"++_pre",0,0,((thisObj->setValue(thisEObj->toDouble()+1.0)),thisEObj))

	//! [ESMF] --Number
	ES_MFUN(typeObject,Number,"--_pre",0,0,(thisObj->setValue(thisEObj->toDouble()-1.0),thisEObj))

	//! [ESMF] Number += Number2
	ES_MFUN(typeObject,Number,"+=",1,1,(thisObj->setValue(thisEObj->toDouble()+parameter[0].to<double>(rt)),thisEObj))

	//! [ESMF] Numbern -= Number2
	ES_MFUN(typeObject,Number,"-=",1,1,(thisObj->setValue(thisEObj->toDouble()-parameter[0].to<double>(rt)),thisEObj))

	//! [ESMF] Numbern *= Number2
	ES_MFUN(typeObject,Number,"*=",1,1,(thisObj->setValue(thisEObj->toDouble()*parameter[0].to<double>(rt)),thisEObj))

	//! [ESMF] Numbern /= Number2
	ES_MFUNCTION(typeObject,Number,"/=",1,1,{
		const double d = parameter[0].to<double>(rt);
		if(d==0){
			rt.setException("Division by zero");
			return nullptr;
		}
		thisObj->setValue(thisObj->getValue()/d);
		return thisObj;
	})

	//! [ESMF] Numbern %= Number2
	ES_MFUNCTION(typeObject,Number,"%=",1,1,{
		const double d = parameter[0].to<double>(rt);
		if(d==0){
			rt.setException("Modulo with zero");
			return nullptr;
		}
		thisObj->setValue(thisObj->modulo(d));
		return thisObj;
	})

	//! [ESMF] Numbern |= Number2
	ES_MFUN(typeObject,Number,"|=",1,1,(thisObj->setValue(thisEObj->toInt() | parameter[0].to<int>(rt)) ,thisEObj))

	//! [ESMF] Numbern &= Number2
	ES_MFUN(typeObject,Number,"&=",1,1,(thisObj->setValue(thisEObj->toInt() & parameter[0].to<int>(rt)) ,thisEObj))

	//! [ESMF] Numbern ^= Number2
	ES_MFUN(typeObject,Number,"^=",1,1,(thisObj->setValue(thisEObj->toInt() ^ parameter[0].to<int>(rt)) ,thisEObj))


// ~=

	//- Comparisons

	//! [ESMF] Numbern > Number2
	ES_FUN(typeObject,">",1,1,thisEObj->toDouble()>parameter[0].to<double>(rt))

	//! [ESMF] Numbern >= Number2
	ES_FUN(typeObject,">=",1,1,thisEObj->toDouble()>=parameter[0].to<double>(rt))

	//! [ESMF] Numbern < Number2
	ES_FUN(typeObject,"<",1,1,thisEObj->toDouble()<parameter[0].to<double>(rt))

	//! [ESMF] Numbern <= Number2
	ES_FUN(typeObject,"<=",1,1,thisEObj->toDouble()<=parameter[0].to<double>(rt))

	//! [ESMF] Bool (Numbern ~= Number2)
	ES_FUN(typeObject,"~=",1,1, Number::matches(thisEObj->toFloat(), parameter[0].to<float>(rt)))

	// - Misc

	//! [ESMF] Number Number.abs()
	ES_FUNCTION2(typeObject,"abs",0,0,{
		const double d = thisEObj->toDouble();
		return  d>0?d:-d;
	})

	//! [ESMF] Number Number.acos
	ES_FUN(typeObject,"acos",0,0,std::acos(thisEObj->toDouble()))

	//! [ESMF] Number Number.asin
	ES_FUN(typeObject,"asin",0,0,std::asin(thisEObj->toDouble()))

	//! [ESMF] Number Number.atan
	ES_FUN(typeObject,"atan",0,0,std::atan(thisEObj->toDouble()))

	//! [ESMF] Number Number.ceil()
	ES_FUN(typeObject,"ceil",0,0,std::ceil( thisEObj->toDouble()))

	//! [ESMF] Number Number.cos
	ES_FUN(typeObject,"cos",0,0,std::cos(thisEObj->toDouble()))

	//! [ESMF] Number Number.clamp(min,max)
	ES_FUNCTION2(typeObject,"clamp",2,2, {
		const double d = thisEObj->toDouble();
		const double min = parameter[0].to<double>(rt);
		if(d<=min)
			return min;
		const double max = parameter[1].to<double>(rt);
		return d<=max ? d : max ;
	})

	//! [ESMF] String Number.degToRad()
	ES_FUN(typeObject,"degToRad",0,0,(thisEObj->toFloat()*M_PI/180.0))

	//! [ESMF] Number Number.floor()
	ES_FUN(typeObject,"floor",0,0,(std::floor( thisEObj->toDouble())))

	//! [ESMF] String Number.format([Number precision = 3[, Bool scientific = true[, Number width = 0[, String fill='0']]]])
	ES_MFUN(typeObject,Number,"format",0,4,thisObj->format(
			static_cast<std::streamsize >(parameter[0].to<int>(rt,3)), parameter[1].toBool(true),
			static_cast<std::streamsize >(parameter[2].to<int>(rt,0)), parameter[3].toString("0")[0]))

	//! [ESMF] Number Number.ln()
	ES_FUN(typeObject,"ln",0,0,std::log(thisEObj->toDouble()))

	//! [ESMF] Number Number.log([basis = 10])
	ES_FUN(typeObject,"log",0,1,( parameter.count()>0?log(thisEObj->toDouble())/log(parameter[0].to<double>(rt)) : log10(thisEObj->toDouble())))

	//! [ESMF] bool Number.matches(other)
	ES_FUN(typeObject,"matches",1,1, Number::matches(thisEObj->toFloat(), parameter[0].to<float>(rt)))

	//! [ESMF] Number Number.pow(Number)
	ES_FUN(typeObject,"pow",1,1,std::pow( thisEObj->toDouble(),parameter[0].to<double>(rt)))

	//! [ESMF] String Number.radToDeg()
	ES_FUN(typeObject,"radToDeg",0,0,(thisEObj->toDouble()*180.0)/M_PI)

	/*! [ESMF] Number Number.round( [reference = 1.0] )
		@param reference Reference value to which should be rounded:  x.round(reference) ^== reference * round(x/reference)
		@example (123.456).round(0.1) == 123.5
				(123.456).round(5) == 125
				(123.456).round(10) == 120 */
	ES_FUNCTION2(typeObject,"round",0,1,{
		if(parameter.count()==0)
			return std::round( thisEObj->toDouble());
		const double reference = parameter[0].to<double>(rt);
		if(reference==0){
			rt.setException("round with zero");
			return nullptr;
		}
		return std::round(thisEObj->toDouble()/reference) * reference;
	})

	//! [ESMF] +1|-1 Number.sign()
	ES_FUN(typeObject,"sign",0,0,(thisEObj->toFloat()<0)?-1.0:1.0)

	//! [ESMF] Number Number.sin
	ES_FUN(typeObject,"sin",0,0,std::sin(thisEObj->toDouble()))

	//! [ESMF] Number Number.sqrt
	ES_FUN(typeObject,"sqrt",0,0,std::sqrt( thisEObj->toDouble()))

	//! [ESMF] Number Number.tan
	ES_FUN(typeObject,"tan",0,0,std::tan( thisEObj->toDouble()))

	//! [ESMF] String Number.toHex()
	ES_FUNCTION2(typeObject,"toHex",0,0,{
		std::ostringstream sprinter;
		sprinter << std::hex << "0x"<<thisEObj->toInt();
		return sprinter.str();
	})

	//! [ESMF] String Number.toIntStr()
	ES_FUNCTION2(typeObject,"toIntStr",0,0,{
		std::ostringstream sprinter;
		sprinter <<thisEObj->toInt();
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
