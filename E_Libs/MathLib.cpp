// MathLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "MathLib.h"
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#endif

#include "ext/Rand.h"
#include "../EScript/EScript.h"

namespace EScript{
namespace MathLib{


// ---------------------------------------------------------

//! EWrapper for Rand-Class
class E_RandomNumberGenerator : public ReferenceObject<Rand> {
	ES_PROVIDES_TYPE_NAME(RandomNumberGenerator)
public:
	//! (static)
	static Type * getTypeObject() {
		static Type * typeObject=new Type(Object::getTypeObject());
		return typeObject;
	}
	static void init(EScript::Namespace & globals);

	//! (ctor)
	E_RandomNumberGenerator(uint32_t seed=0):
			ReferenceObject_t(seed,getTypeObject()){}

	//! (dtor)
	virtual ~E_RandomNumberGenerator(){}

	//! ---|> Object
	virtual E_RandomNumberGenerator * clone()const	{	return new E_RandomNumberGenerator(ref().getSeed());	}

};


// ---------------------------------------------------------

//! (static) MathLib init
void init(EScript::Namespace * globals) {
	Namespace * lib=new Namespace();
	declareConstant(globals,"Math",lib);

	declareConstant(lib,"PI",Number::create(M_PI));
	declareConstant(lib,"PI_2",Number::create(M_PI_2));


	//! Number Math.atan2(a,b)
	ESF_DECLARE(lib, "atan2", 2, 2,
				Number::create(std::atan2(parameter[0].toDouble(), parameter[1].toDouble())))


	// init E_RandomNumberGenerator
	E_RandomNumberGenerator::init(*lib);

	// init global E_RandomNumberGenerator-Object
	declareConstant(globals,"Rand",new E_RandomNumberGenerator);

	// ------
}

// ---------------------------------------------------------------

//! (static) init members for E_RandomNumberGenerator
void E_RandomNumberGenerator::init(EScript::Namespace & lib) {

	// E_Rand ---|> [Object]
	Type * typeObject=getTypeObject();
	declareConstant(&lib,getClassName(),typeObject);


	//! [ESF] new RandomNumberGenerator( [seed] )
	ESF_DECLARE(typeObject,"_constructor",0,1,(
			new E_RandomNumberGenerator(parameter[0].toInt(0))))

	//! [ESF] [0,1] RandomNumberGenerator.bernoulli(p)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"bernoulli",1,1,(
			Number::create( self->ref().bernoulli(parameter[0].toDouble()))))

	//! [ESF] int RandomNumberGenerator.binomial(n,p)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"binomial",2,2,
				Number::create( self->ref().binomial(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] float RandomNumberGenerator.chisquare(n)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"chisquare",1,1,
				Number::create(self->ref().chisquare(parameter[0].toInt())))

	//! [ESF] int RandomNumberGenerator.equilikely(a,b)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"equilikely",2,2,
				Number::create(self->ref().equilikely(parameter[0].toInt(),parameter[1].toInt())))

	//! [ESF] float RandomNumberGenerator.erlang(n,b)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"erlang",2,2,
				Number::create( self->ref().erlang(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] float RandomNumberGenerator.exponential(m)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"exponential",1,1,
				Number::create(self->ref().exponential(parameter[0].toDouble())))

	//! [ESF] int RandomNumberGenerator.geometric(p)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"geometric",1,1,
				Number::create( self->ref().geometric(parameter[0].toDouble())))

	//! [ESF] int RandomNumberGenerator.getSeed()
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"getSeed",0,0,
				Number::create( self->ref().getSeed()))

	//! [ESF] float RandomNumberGenerator.lognormal(a,b)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"lognormal",2,2,
				Number::create( self->ref().lognormal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] float RandomNumberGenerator.normal(m,s)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"normal",2,2,
				Number::create( self->ref().normal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] int RandomNumberGenerator.pascal(n,p)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"pascal",2,2,
				Number::create(self->ref().pascal(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] int RandomNumberGenerator.poisson(m)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"poisson",1,1,
				Number::create(self->ref().poisson(parameter[0].toDouble())))

	//! [ESF] float RandomNumberGenerator.random()      [0.0 ... 1.0]
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"random",0,0,
				Number::create(self->ref().random()))

	//! [ESF] self RandomNumberGenerator.setSeed(Number)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"setSeed",1,1,
				(self->ref().setSeed(parameter[0].toInt()),self))

	//! [ESF]float RandomNumberGenerator.student(n)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"student",1,1,
				Number::create(self->ref().student(parameter[0].toInt())))

	//! [ESF] float RandomNumberGenerator.uniform(a,b)
	ESMF_DECLARE(typeObject,E_RandomNumberGenerator,"uniform",2,2,
				Number::create( self->ref().uniform(parameter[0].toDouble(),parameter[1].toDouble())))

}
// ---------------------------------------------------------------

}
}
