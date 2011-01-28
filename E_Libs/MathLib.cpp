// MathLib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "MathLib.h"
#include <cmath>
#include "ext/Rand.h"

#include "../EScript/EScript.h"

using namespace EScript;

static Rand myRand;

// ---------------------------------------------------------

//! (static)
void MathLib::init(EScript::Namespace * globals) {
	Namespace * lib=new Namespace();
	declareConstant(globals,"Math",lib);

	declareConstant(lib,"PI",Number::create(M_PI));
	declareConstant(lib,"PI_2",Number::create(M_PI_2));

	/*!	[ESF] number Math.rand(max,[seed])
		based on // http://www.cs.wm.edu/~va/software/park/	*/
	ES_FUNCTION_DECLARE(lib,"rand",1,2,{
		static long seed=1;
		if (parameter.count()>1)
			seed=static_cast<unsigned int>(parameter[1].toInt());
		const long Q = 2147483647 / 48271;
		const long R = 2147483647 % 48271;
		long t = 48271 * (seed % Q) - R * (seed / Q);
		if (t > 0)
			seed = t;
		else
			seed = t + 2147483647;
		return Number::create( seed%(static_cast<unsigned int>(parameter[0].toInt()+1)));
	})

	//! Number Math.atan2(a,b)
	ESF_DECLARE(lib, "atan2", 2, 2,
				Number::create(std::atan2(parameter[0].toDouble(), parameter[1].toDouble())))

	// ----------------------------------

	Namespace * randLib=new Namespace();
	declareConstant(globals,"Rand",randLib);

	//! [ESF] [0,1] Rand.bernoulli(p)
	ESF_DECLARE(randLib,"bernoulli",1,1,
				Number::create(myRand.bernoulli(parameter[0].toDouble())))

	//! [ESF] int Rand.binomial(n,p)
	ESF_DECLARE(randLib,"binomial",2,2,
				Number::create( myRand.binomial(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] int Rand.equilikely(a,b)
	ESF_DECLARE(randLib,"equilikely",2,2,
				Number::create(myRand.equilikely(parameter[0].toInt(),parameter[1].toInt())))

	//! [ESF] int Rand.geometric(p)
	ESF_DECLARE(randLib,"geometric",1,1,
				Number::create( myRand.geometric(parameter[0].toDouble())))

	//! [ESF] int Rand.pascal(n,p)
	ESF_DECLARE(randLib,"pascal",2,2,
				Number::create(myRand.pascal(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] int Rand.poisson(m)
	ESF_DECLARE(randLib,"poisson",1,1,
				Number::create(myRand.poisson(parameter[0].toDouble())))

	//! [ESF] float Rand.uniform(a,b)
	ESF_DECLARE(randLib,"uniform",2,2,
				Number::create( myRand.uniform(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] float Rand.exponential(m)
	ESF_DECLARE(randLib,"exponential",1,1,
				Number::create(myRand.exponential(parameter[0].toDouble())))

	//! [ESF] float Rand.erlang(n,b)
	ESF_DECLARE(randLib,"erlang",2,2,
				Number::create( myRand.erlang(parameter[0].toInt(),parameter[1].toDouble())))

	//! [ESF] float Rand.normal(m,s)
	ESF_DECLARE(randLib,"normal",2,2,
				Number::create( myRand.normal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] float Rand.lognormal(a,b)
	ESF_DECLARE(randLib,"lognormal",2,2,
				Number::create( myRand.lognormal(parameter[0].toDouble(),parameter[1].toDouble())))

	//! [ESF] float Rand.chisquare(n)
	ESF_DECLARE(randLib,"chisquare",1,1,
				Number::create(myRand.chisquare(parameter[0].toInt())))

	//! [ESF]float Rand.student(n)
	ESF_DECLARE(randLib,"student",1,1,
				Number::create(myRand.student(parameter[0].toInt())))

}
