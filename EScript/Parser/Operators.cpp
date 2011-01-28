#include "Operators.h"
#include <vector>
namespace EScript {


/*! (static) */
Operator::operatorMap_t Operator::ops;

/*! (static) */
const Operator * Operator::getOperator(identifierId id) {
	if(ops.empty()){
		int p=-1;
		declareOperator(p   ,"UNKNOWN");
		declareOperator(++p ,"_");
		declareOperator(++p ,".");
		declareOperator(p   ,"[");
		declareOperator(p   ,"]");
		declareOperator(p   ,"(");
		declareOperator(p   ,")");

		declareOperator(++p ,"new");
		declareOperator(++p ,"fn",R);
		declareOperator(++p ,"lambda",R);

		declareOperator(++p ,"++",R);
		declareOperator(p   ,"--",R);
		declareOperator(p   ,"_-",R); // 2+-1
		declareOperator(p   ,"~",R);
		declareOperator(p   ,"!",R);

		declareOperator(++p ,"*");
		declareOperator(p ,"/");
		declareOperator(++p ,"%");
		declareOperator(++p ,"+");
		declareOperator(p ,"-");
		declareOperator(++p ,"<");
		declareOperator(p   ,">");
		declareOperator(p   ,">=");
		declareOperator(p   ,"<=");
		declareOperator(++p ,"==");
		declareOperator(p   ,"!=");
		declareOperator(p   ,"===");
		declareOperator(p   ,"!==");
		declareOperator(p   ,"---|>");
		declareOperator(p   ,"isA");
		declareOperator(p   ,"->"); // ??
		declareOperator(++p ,"&");
		declareOperator(++p ,"^");
		declareOperator(++p ,"|");
		declareOperator(++p ,"&&");
		declareOperator(++p ,"||");

		//declareOperator(++p ,":");
		declareOperator(++p ,"?");//R
		declareOperator(++p ,"=",R);
		declareOperator(++p ,":=",R);
		declareOperator(++p ,"::=",R);
		declareOperator(p   ,"*=",R);
		declareOperator(p   ,"/=",R);
		declareOperator(p   ,"%=",R);
		declareOperator(p   ,"+=",R);
		declareOperator(p   ,"-=",R);
		declareOperator(p   ,"&=",R);
		declareOperator(p   ,"^=",R);
		declareOperator(p   ,"|=",R);
		declareOperator(++p ,",");
	}
	operatorMap_t::const_iterator it=ops.find(id);
	return  it==ops.end() ? NULL : it->second;
}

/*! (static) */
const Operator * Operator::getOperator(const std::string & op) {
	return getOperator(stringToIdentifierId(op));
}

/*! (static) */
void Operator::declareOperator(int precedence,const std::string & op,associativity_t associativity){
	identifierId id=stringToIdentifierId(op);
	ops[id]=new Operator(id,precedence,op,associativity);
}

/*!	[ctor] */
Operator::Operator(identifierId _id,int _precedence,const std::string & _s,associativity_t _associativity):
		id(_id),precedence(_precedence),s(_s),associativity(_associativity) {
}

}
