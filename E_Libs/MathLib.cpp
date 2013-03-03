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

#include "../EScript/Basics.h"
#include "../EScript/StdObjects.h"
#include "../EScript/Objects/ReferenceObject.h"
#include <ctime>
#include <random>

namespace EScript{
namespace MathLib{


// ---------------------------------------------------------

//! EWrapper for C++ random number engine
class E_RandomNumberGenerator : public ReferenceObject<std::mt19937> {
	ES_PROVIDES_TYPE_NAME(RandomNumberGenerator)
	public:
		//! (static)
		static Type * getTypeObject() {
			static Type * typeObject = new Type(Object::getTypeObject());
			return typeObject;
		}
		static void init(EScript::Namespace & globals);

		//! (ctor)
		E_RandomNumberGenerator(uint32_t seed) :
			ReferenceObject_t(std::mt19937(seed), getTypeObject()) {
		}
		
		E_RandomNumberGenerator(const std::mt19937 & engine) :
			ReferenceObject_t(engine, getTypeObject()) {
		}

		//! (dtor)
		virtual ~E_RandomNumberGenerator() {
		}

		//! ---|> Object
		virtual E_RandomNumberGenerator * clone() const {
			return new E_RandomNumberGenerator(ref());
		}
};


// ---------------------------------------------------------

//! (static) MathLib init
void init(EScript::Namespace * globals) {
	Namespace * lib = new Namespace;
	declareConstant(globals,"Math",lib);

	declareConstant(lib,"PI",	M_PI);
	declareConstant(lib,"PI_2",	M_PI_2);


	//! Number Math.atan2(a,b)
	ES_FUN(lib, "atan2", 2, 2,
				std::atan2(parameter[0].to<double>(rt), parameter[1].to<double>(rt)))


	// init E_RandomNumberGenerator
	E_RandomNumberGenerator::init(*lib);

	// init global E_RandomNumberGenerator-Object
	declareConstant(globals, "Rand", new E_RandomNumberGenerator(static_cast<unsigned int>(std::time(nullptr))));

	// ------
}

// ---------------------------------------------------------------

//! (static) init members for E_RandomNumberGenerator
void E_RandomNumberGenerator::init(EScript::Namespace & lib) {
	// E_Rand ---|> [Object]
	Type * typeObject = getTypeObject();
	declareConstant(&lib, getClassName(), typeObject);

	//! [ESF] new RandomNumberGenerator( [seed] )
	ES_CTOR(typeObject,0,1,new E_RandomNumberGenerator(parameter[0].toInt(0)))

	//! [ESMF] [0, 1] RandomNumberGenerator.bernoulli(p)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "bernoulli", 1, 1,
				 std::bernoulli_distribution(parameter[0].to<double>(rt))(**thisObj) ? 1 : 0)

	//! [ESMF] Number RandomNumberGenerator.binomial(n,p)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "binomial", 2, 2,
				 std::binomial_distribution<int>(parameter[0].to<int>(rt), parameter[1].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.categorical(Array weights)
	ES_MFUNCTION(typeObject, E_RandomNumberGenerator, "categorical", 1, 1, {
		Array * array = assertType<EScript::Array>(rt, parameter[0]);
		std::vector<double> weights;
		weights.reserve(array->size());
		for(const auto & element : *array) {
			weights.push_back(element->toDouble());
		}
		return std::discrete_distribution<int>(weights.begin(), weights.end())(**thisObj);
	})
	
	//! [ESMF] Number RandomNumberGenerator.chisquare(n)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "chisquare", 1, 1,
				 std::chi_squared_distribution<double>(parameter[0].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.equilikely(a,b)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "equilikely", 2, 2,
				 std::uniform_int_distribution<int>(parameter[0].to<int>(rt), parameter[1].to<int>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.exponential(m)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "exponential", 1, 1,
				 std::exponential_distribution<double>(parameter[0].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.geometric(p)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "geometric", 1, 1,
				 std::geometric_distribution<int>(parameter[0].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.lognormal(a,b)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "lognormal", 2, 2,
				 std::lognormal_distribution<double>(parameter[0].to<double>(rt), parameter[1].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.normal(m,s)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "normal", 2, 2,
				 std::normal_distribution<double>(parameter[0].to<double>(rt), parameter[1].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.pascal(n,p)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "pascal", 2, 2,
				 std::negative_binomial_distribution<int>(parameter[0].to<int>(rt), parameter[1].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.poisson(m)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "poisson", 1, 1,
				 std::poisson_distribution<int>(parameter[0].to<double>(rt))(**thisObj))

	//! [ESMF] thisObj RandomNumberGenerator.setSeed(Number)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "setSeed", 1, 1,
				 ((**thisObj).seed(parameter[0].to<int>(rt)), thisEObj))

	//! [ESMF] Number RandomNumberGenerator.student(n)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "student", 1, 1,
				 std::student_t_distribution<double>(parameter[0].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.uniform(a,b)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "uniform", 2, 2,
				 std::uniform_real_distribution<double>(parameter[0].to<double>(rt), parameter[1].to<double>(rt))(**thisObj))

	//! [ESMF] Number RandomNumberGenerator.weibull(shape, scale)
	ES_MFUN(typeObject, E_RandomNumberGenerator, "weibull", 2, 2,
				 std::weibull_distribution<double>(parameter[0].to<double>(rt), parameter[1].to<double>(rt))(**thisObj))

}

}
}
