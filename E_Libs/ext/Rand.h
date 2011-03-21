// Rand.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef RAND_H
#define RAND_H
#include <stdint.h>

/**
 *  Rand - a pseudo random number generator
 **
 *  Based on the library "rvgs.c" by Steve Park & Dave Geyer
 *  Reference: http://www.cs.wm.edu/~va/software/park/
 */
class Rand{
	public:
		typedef int32_t intType;
		typedef double floatType;

		Rand(intType initialSeed=0);
		~Rand();
		Rand(const Rand& other);

		Rand& operator=(const Rand& other);
		bool operator==(const Rand& other)const	{	return seed == other.seed;	}

		//-----------------
		// Seed
		intType getSeed() const 		{	return seed;	}
		void setSeed(intType newSeed) 	{	seed = newSeed;	}
		void generateSeed();

		//-----------------
		// Number geneartion (from rngs.h)
		floatType random();

		//-----------------
		// Distributions (from rvgs.h)
		intType bernoulli(floatType p);
		intType binomial(intType n, floatType p);
		intType equilikely(intType a, intType b);
		intType geometric(floatType p);
		intType pascal(intType n, floatType p);
		intType poisson(floatType m);

		floatType uniform(floatType a, floatType b);
		floatType exponential(floatType m);
		floatType erlang(intType n, floatType b);
		floatType normal(floatType m, floatType s);
		floatType lognormal(floatType a, floatType b);
		floatType chisquare(intType n);
		floatType student(intType n);

	private:
		intType seed;
};

#endif // RAND_H
