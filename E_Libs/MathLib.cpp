// MathLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "MathLib.h"
#include <cmath>

#include "ext/Rand.h"
#include "../EScript/EScript.h"

namespace EScript{
namespace MathLib{


// ---------------------------------------------------------

//! EWrapper for Rand-Class
class E_RandNumGenerator : public ReferenceObject<Rand> {
	ES_PROVIDES_TYPE_NAME(RandNumGenerator);
public:
	//! (static)
	static Type * getTypeObject() {
		static Type * typeObject=new Type(Object::getTypeObject());
		return typeObject;
	}
	static void init(EScript::Namespace & globals);

	//! (ctor)
	E_RandNumGenerator(uint32_t seed=0):
			ReferenceObject<Rand>(seed,getTypeObject()){}

	//! (dtor)
	virtual ~E_RandNumGenerator(){}
};


// ---------------------------------------------------------

//! (static) MathLib init
void init(EScript::Namespace * globals) {
	Namespace * lib=new Namespace();
	declareConstant(globals,"Math",lib);

	declareConstant(lib,"PI",Number::create(M_PI));
	declareConstant(lib,"PI_2",Number::create(M_PI_2));
//
//	/*!	[ESF] number Math.rand(max,[seed])
//		based on // http://www.cs.wm.edu/~va/software/park/	*/
//	ES_FUNCTION_DECLARE(lib,"rand",1,2,{
//		static long seed=1;
//		if (parameter.count()>1)
//			seed=static_cast<unsigned int>(parameter[1].toInt());
//		const long Q = 2147483647 / 48271;
//		const long R = 2147483647 % 48271;
//		long t = 48271 * (seed % Q) - R * (seed / Q);
//		if (t > 0)
//			seed = t;
//		else
//			seed = t + 2147483647;
//		return Number::create( seed%(static_cast<unsigned int>(parameter[0].toInt()+1)));
//	})

	//! Number Math.atan2(a,b)
	ESF_DECLARE(lib, "atan2", 2, 2,
				Number::create(std::atan2(parameter[0].toDouble(), parameter[1].toDouble())))


	// init E_RandNumGenerator
	E_RandNumGenerator::init(*lib);

	// init global E_RandNumGenerator-Object
	declareConstant(globals,"Rand",new E_RandNumGenerator);

	// ------
}

// ---------------------------------------------------------------

//! (static) init members for E_RandNumGenerator
void E_RandNumGenerator::init(EScript::Namespace & lib) {

	// E_Rand ---|> [Object]
	Type * typeObject=getTypeObject();
	declareConstant(&lib,getClassName(),typeObject);


	//! [ESF] new RandNumGenerator( [seed] )
	ESF_DECLARE(typeObject,"_constructor",0,1,(
			new E_RandNumGenerator(parameter[0].toInt(0))))

	//! [ESF] [0,1] RandNumGenerator.bernoulli(p)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"bernoulli",1,1,(
			Number::create( self->ref().bernoulli(parameter[0].toDouble()))))

	//! [ESF] int RandNumGenerator.binomial(n,p)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"binomial",2,2,
				Number::create( self->ref().binomial(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] float RandNumGenerator.chisquare(n)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"chisquare",1,1,
				Number::create(self->ref().chisquare(parameter[0].toInt())))

	//! [ESF] int RandNumGenerator.equilikely(a,b)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"equilikely",2,2,
				Number::create(self->ref().equilikely(parameter[0].toInt(),parameter[1].toInt())))

	//! [ESF] float RandNumGenerator.erlang(n,b)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"erlang",2,2,
				Number::create( self->ref().erlang(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] float RandNumGenerator.exponential(m)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"exponential",1,1,
				Number::create(self->ref().exponential(parameter[0].toDouble())))

	//! [ESF] int RandNumGenerator.geometric(p)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"geometric",1,1,
				Number::create( self->ref().geometric(parameter[0].toDouble())))

	//! [ESF] float RandNumGenerator.lognormal(a,b)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"lognormal",2,2,
				Number::create( self->ref().lognormal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] float RandNumGenerator.normal(m,s)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"normal",2,2,
				Number::create( self->ref().normal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] int RandNumGenerator.pascal(n,p)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"pascal",2,2,
				Number::create(self->ref().pascal(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] int RandNumGenerator.poisson(m)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"poisson",1,1,
				Number::create(self->ref().poisson(parameter[0].toDouble())))

	//! [ESF] float RandNumGenerator.random()      [0.0 ... 1.0]
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"random",0,0,
				Number::create(self->ref().random()))

	//! [ESF]float RandNumGenerator.student(n)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"student",1,1,
				Number::create(self->ref().student(parameter[0].toInt())))

	//! [ESF] float RandNumGenerator.uniform(a,b)
	ESMF_DECLARE(typeObject,E_RandNumGenerator,"uniform",2,2,
				Number::create( self->ref().uniform(parameter[0].toDouble(),parameter[1].toDouble())))

}
// ---------------------------------------------------------------

}
}
