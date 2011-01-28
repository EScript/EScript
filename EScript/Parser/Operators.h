// Operators.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef OPERATOR_H
#define OPERATOR_H

#include "../Utils/Hashing.h"
#include <string>
#include <map>

namespace EScript {


/*! Operator    */
class Operator {
	public:
		enum associativity_t{	L=0,R=1	};

		static const Operator * getOperator(identifierId id);
		static const Operator * getOperator(const std::string & s);

		std::string getString()const			{	return s;	}
		identifierId getId()const				{	return id;	}
		int getPrecedence()const				{	return precedence;	}
		associativity_t getAssociativity()const	{	return associativity;	}

	private:
		typedef std::map<identifierId,Operator*> operatorMap_t;
		static operatorMap_t ops;
		static void declareOperator(int precedence,const std::string & op,associativity_t associativity=L);

		Operator(identifierId id,int precedence,const std::string & _s,associativity_t associativity=L);

		identifierId id;
		int precedence;
		std::string s;
		associativity_t associativity;
};
}

#endif // OPERATOR_H
