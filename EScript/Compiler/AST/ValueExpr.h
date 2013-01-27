// ValueExpr.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_VALUEEXPR_H
#define ES_VALUEEXPR_H

#include "ASTNode.h"
#include <string>

namespace EScript {
namespace AST {

//! [BoolValueExpr]  ---|> [ASTNode]
class BoolValueExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(BoolValueExpr)
	public:
		BoolValueExpr(bool _value,int _line=-1) : 
				ASTNode(TYPE_VALUE_BOOL,_line),value(_value) {}
		virtual ~BoolValueExpr(){}
		bool getValue()const	{	return value;	}
	private:
		bool value;
};
//! [IdentifierValueExpr]  ---|> [ASTNode]
class IdentifierValueExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(IdentifierValueExpr)
	public:
		IdentifierValueExpr(const StringId & _value,int _line=-1) : 
				ASTNode(TYPE_VALUE_IDENTIFIER,_line),value(_value) {}
		virtual ~IdentifierValueExpr(){}
		const StringId & getValue()const	{	return value;	}
	private:
		StringId value;
};
//! [NumberValueExpr]  ---|> [ASTNode]
class NumberValueExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(NumberValueExpr)
	public:
		NumberValueExpr(double _value,int _line=-1) : 
				ASTNode(TYPE_VALUE_FLOATING_POINT,_line),value(_value) {}
		virtual ~NumberValueExpr(){}
		double getValue()const	{	return value;	}
		void setValue(const double & v)	{	value = v;	}
	private:
		double value;
};
//! [StringValueExpr]  ---|> [ASTNode]
class StringValueExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(StringValueExpr)
	public:
		StringValueExpr(const std::string & _value,int _line=-1) : 
				ASTNode(TYPE_VALUE_STRING,_line),value(_value) {}
		virtual ~StringValueExpr(){}
		const std::string & getValue()const	{	return value;	}
	private:
		std::string value;
};
//! [VoidValueExpr]  ---|> [ASTNode]
class VoidValueExpr : public ASTNode {
		ES_PROVIDES_TYPE_NAME(VoidValueExpr)
	public:
		VoidValueExpr(int _line=-1) : 
				ASTNode(TYPE_VALUE_VOID,_line) {}
		virtual ~VoidValueExpr(){}
};
}
}

#endif // ES_VALUEEXPR_H
