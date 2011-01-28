#include "MathLib.h"
#include <cmath>
#include "ext/Rand.h"

#include "../EScript/EScript.h"

using namespace EScript;

static Rand myRand=Rand();

/**
 * [ESF] number rand(max,[seed])
 * based on // http://www.cs.wm.edu/~va/software/park/
 */
ES_FUNCTION(esf_rand) {
    assertParamCount(runtime,parameter.count(),1,2);

    static long seed=1;

    if (parameter.count()>1) seed=static_cast<unsigned int>(parameter[1].toInt());

    const long Q = 2147483647 / 48271;
    const long R = 2147483647 % 48271;
    long t = 48271 * (seed % Q) - R * (seed / Q);
    if (t > 0)
        seed = t;
    else
        seed = t + 2147483647;
    return Number::create( seed%(static_cast<unsigned int>(parameter[0].toInt()+1)));

//    static unsigned int seed=1;
//    if(nr>1)
//        seed=parameter[1].toInt();
//    unsigned int max=static_cast<unsigned int>(parameter[0].toInt());
////    if (nr>1) seed=((static_cast<unsigned int>(parameter[1].toInt())^0x23482012))*29;
////
////    unsigned int result=(seed^0xea731829+1)*17;
////    seed=result;
//    seed^=(((seed)*1234393)% 0xffffff);
//    //std::cout << " "<<hv<<" ";
//    return Number::create((int)(seed%(max+1)));
}

/*!	[ESF] [0,1] Rand.bernoulli(p)	*/
ESF(esf_Rand_bernoulli,1,1, Number::create(myRand.bernoulli(parameter[0].toDouble())))

/*!	[ESF] int Rand.binomial(n,p)	*/
ESF(esf_Rand_binomial,2,2, Number::create( myRand.binomial(parameter[0].toInt(),parameter[1].toDouble())))

/*!	[ESF] int Rand.equilikely(a,b)	*/
ESF(esf_Rand_equilikely,2,2, Number::create(myRand.equilikely(parameter[0].toInt(),parameter[1].toInt())))

/*!	[ESF] int Rand.geometric(p)	*/
ESF(esf_Rand_geometric,1,1, Number::create( myRand.geometric(parameter[0].toDouble())))

/*!	[ESF] int Rand.pascal(n,p)	*/
ESF(esf_Rand_pascal,2,2, Number::create(myRand.pascal(parameter[0].toInt(),parameter[1].toDouble())))

/*!	[ESF] int Rand.poisson(m)	*/
ESF(esf_Rand_poisson,1,1, Number::create(myRand.poisson(parameter[0].toDouble())))

/*!	[ESF] float Rand.uniform(a,b)	*/
ESF(esf_Rand_uniform,2,2, Number::create( myRand.uniform(parameter[0].toDouble(),parameter[1].toDouble())))

/*!	[ESF] float Rand.exponential(m)	*/
ESF(esf_Rand_exponential,1,1, Number::create(myRand.exponential(parameter[0].toDouble())))

/*!	[ESF] float Rand.erlang(n,b)	*/
ESF(esf_Rand_erlang,2,2, Number::create( myRand.erlang(parameter[0].toInt(),parameter[1].toDouble())))

/*!	[ESF] float Rand.normal(m,s)	*/
ESF(esf_Rand_normal,2,2, Number::create( myRand.normal(parameter[0].toDouble(),parameter[1].toDouble())))

/*!	[ESF] float Rand.lognormal(a,b)	*/
ESF(esf_Rand_lognormal,2,2, Number::create( myRand.lognormal(parameter[0].toDouble(),parameter[1].toDouble())))

/*!	[ESF] float Rand.chisquare(n)	*/
ESF(esf_Rand_chisquare,1,1, Number::create(myRand.chisquare(parameter[0].toInt())))

/*!	[ESF]float Rand.student(n)	*/
ESF(esf_Rand_student,1,1, Number::create(myRand.student(parameter[0].toInt())))

// ---------------------------------------------------------

void MathLib::init(EScript::Namespace * globals) {
    Namespace * lib=new Namespace();
	declareConstant(globals,"Math",lib);

    declareFunction(lib,"rand",esf_rand);
    declareConstant(lib,"PI",Number::create(M_PI));
    declareConstant(lib,"PI_2",Number::create(M_PI_2));

    ESF_DECLARE(lib, "atan2", 2, 2, Number::create(std::atan2(parameter[0].toDouble(), parameter[1].toDouble())));
	// ----

    Namespace * randLib=new Namespace();
    declareConstant(globals,"Rand",randLib);
    declareFunction(randLib,"bernoulli",esf_Rand_bernoulli);
    declareFunction(randLib,"binomial",esf_Rand_binomial);
    declareFunction(randLib,"equilikely",esf_Rand_equilikely);
    declareFunction(randLib,"geometric",esf_Rand_geometric);
    declareFunction(randLib,"pascal",esf_Rand_pascal);
    declareFunction(randLib,"poisson",esf_Rand_poisson);

    declareFunction(randLib,"uniform",esf_Rand_uniform);
    declareFunction(randLib,"exponential",esf_Rand_exponential);
    declareFunction(randLib,"erlang",esf_Rand_erlang);
    declareFunction(randLib,"normal",esf_Rand_normal);
    declareFunction(randLib,"lognormal",esf_Rand_lognormal);
    declareFunction(randLib,"chisquare",esf_Rand_chisquare);
    declareFunction(randLib,"student",esf_Rand_student);
}

