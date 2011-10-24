// Rand.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Rand.h"
/**
 *  Rand - a pseudo random number generator
 **
 *  Based on the library "rvgs.c" by Steve Park & Dave Geyer
 *  Reference: http://www.cs.wm.edu/~va/software/park/
 */
#include <math.h>
#include <time.h>


#define MODULUS    2147483647 /* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271      /* DON'T CHANGE THIS VALUE                  */
#define CHECK      399268537  /* DON'T CHANGE THIS VALUE                  */
#define STREAMS    256        /* # of streams, DON'T CHANGE THIS VALUE    */
#define A256       22925      /* jump multiplier, DON'T CHANGE THIS VALUE */
#define DEFAULT    123456789  /* initial seed, use 0 < DEFAULT < MODULUS  */

namespace EScript {

//! (ctor)
Rand::Rand(intType initialSeed/*=0*/) {
	if (initialSeed<=0)
		generateSeed();
	else
		setSeed(initialSeed);
	//ctor
}

//! (ctor)
Rand::Rand(const Rand& other):seed(other.seed) {
	//copy ctor
}

//! (dtor)
Rand::~Rand() {
	//dtor
}

Rand& Rand::operator=(const Rand& rhs) {
	if (this == &rhs) return *this; // handle self assignment
	setSeed(rhs.getSeed());
	//assignment operator
	return *this;
}

void Rand::generateSeed(){
	setSeed ( static_cast<unsigned long>(time( reinterpret_cast<time_t *>(NULL))) % MODULUS);
}

/**
 * Random returns a pseudo-random real number uniformly distributed
 * between 0.0 and 1.0.
 */
Rand::floatType Rand::random() {
	const intType Q = MODULUS / MULTIPLIER;
	const intType R = MODULUS % MULTIPLIER;
	intType t = MULTIPLIER * ( getSeed() % Q) - R * (getSeed()  / Q);
	if (t > 0)
		setSeed(t);
	else
		setSeed(t + MODULUS);
	return ( static_cast<floatType>(getSeed()) / MODULUS);
}


/**
 * Returns 1 with probability p or 0 with probability 1 - p.
 * NOTE: use 0.0 < p < 1.0
 *
 */
Rand::intType Rand::bernoulli(floatType p){
	return ((random() < (1.0 - p)) ? 0 : 1);
}

/**
 * Returns a binomial distributed integer between 0 and n inclusive.
 * NOTE: use n > 0 and 0.0 < p < 1.0
 *
 */
Rand::intType Rand::binomial(intType n, floatType p){
	intType i, x = 0;

	for (i = 0; i < n; ++i)
		x += bernoulli(p);
	return (x);
}

/**
 * Returns an equilikely distributed integer between a and b inclusive.
 * NOTE: use a < b
 *
 */
Rand::intType Rand::equilikely(intType a, intType b){
	return a + static_cast<intType>((b - a + 1) * random());
}

/**
 * Returns a geometric distributed non-negative integer.
 * NOTE: use 0.0 < p < 1.0
 *
 */
Rand::intType Rand::geometric(floatType p){
	return static_cast<intType>(log(1.0 - random()) / log(p));
}

/**
 * Returns a Pascal distributed non-negative integer.
 * NOTE: use n > 0 and 0.0 < p < 1.0
 */
Rand::intType Rand::pascal(intType n, floatType p){
	intType i, x = 0;

	for (i = 0; i < n; ++i)
		x += geometric(p);
	return x;
}

/**
 * Returns a Poisson distributed non-negative integer.
 * NOTE: use m > 0
 */
Rand::intType Rand::poisson(floatType m){
	floatType t = 0.0;
	intType   x = 0;

	while (t < m) {
		t += exponential(1.0);
		x++;
	}
	return x - 1;
}

/**
 * Returns a uniformly distributed real number between a and b.
 * NOTE: use a < b
 */
Rand::floatType Rand::uniform(floatType a, floatType b){
	return a + (b - a) * random();
}

/**
 * Returns an exponentially distributed positive real number.
 * NOTE: use m > 0.0
 *
 */
Rand::floatType Rand::exponential(floatType m){
	return -m * log(1.0 - random());
}

/**
 * Returns an Erlang distributed positive real number.
 * NOTE: use n > 0 and b > 0.0
 */
Rand::floatType Rand::erlang(intType n, floatType b){
	floatType x = 0.0;
	for (intType i = 0; i < n; ++i)
		x += exponential(b);
	return x;
}

/**
 * Returns a normal (Gaussian) distributed real number.
 * NOTE: use s > 0.0
 *
 * Uses a very accurate approximation of the normal idf due to Odeh & Evans,
 * J. Applied Statistics, 1974, vol 23, pp 96-97.
 *
 */
 Rand::floatType Rand::normal(floatType m, floatType s){
	const floatType p0 = 0.322232431088;
	const floatType q0 = 0.099348462606;
	const floatType p1 = 1.0;
	const floatType q1 = 0.588581570495;
	const floatType p2 = 0.342242088547;
	const floatType q2 = 0.531103462366;
	const floatType p3 = 0.204231210245e-1;
	const floatType q3 = 0.103537752850;
	const floatType p4 = 0.453642210148e-4;
	const floatType q4 = 0.385607006340e-2;
	floatType u, t, p, q, z;

	u   = random();
	if (u < 0.5)
		t = sqrt(-2.0 * log(u));
	else
		t = sqrt(-2.0 * log(1.0 - u));
	p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
	q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));
	if (u < 0.5)
		z = (p / q) - t;
	else
		z = t - (p / q);
	return m + s * z;
}

/**
 * Returns a lognormal distributed positive real number.
 * NOTE: use b > 0.0
 */
Rand::floatType Rand::lognormal(floatType a, floatType b){
	return exp(a + b * normal(0.0, 1.0));
}

/**
 * Returns a chi-square distributed positive real number.
 * NOTE: use n > 0
 */
Rand::floatType Rand::chisquare(intType n){
	intType   i;
	floatType z, x = 0.0;

	for (i = 0; i < n; ++i) {
		z  = normal(0.0, 1.0);
		x += z * z;
	}
	return x;
}

/**
 * Returns a student-t distributed real number.
 * NOTE: use n > 0
 */
Rand::floatType Rand::student(intType n){
	return normal(0.0, 1.0) / sqrt(chisquare(n) / n);
}

}
