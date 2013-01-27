// Statement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_STATEMENT_H
#define ES_STATEMENT_H

#include "../Object.h"
#include "../../Utils/ObjRef.h"

namespace EScript {

namespace AST {
//! [Statement]
class Statement {
	public:
		enum type_t{
			TYPE_EXPRESSION, // contained expression adds to the stack
			TYPE_STATEMENT,   //  default; does not add to the stack
			TYPE_BREAK,
			TYPE_CONTINUE,
			TYPE_RETURN,
			TYPE_YIELD,
			TYPE_THROW,
			TYPE_EXIT,
			TYPE_UNDEFINED
		};

		Statement( type_t _type, ObjPtr _expression) :
				type(_type),expression(_expression),line(-1) {}
		explicit Statement( type_t _type = TYPE_UNDEFINED ) : type(_type),line(-1) {}

		~Statement(){}

		type_t getType()const			{	return type;	}
		ObjPtr getExpression()const		{	return expression;	}

		int getLine()const				{	return line;	}
		void setLine(int newLine)		{	line = newLine;	}

		bool isValid()const				{	return type!=TYPE_UNDEFINED;	}
		bool operator==(const Statement & other) const {
			return type==other.type && expression==other.expression && line==other.line;
		}

		virtual internalTypeId_t _getInternalTypeId()const {	return _TypeIds::TYPE_STATEMENT; }

	private:
		type_t type;
		ObjRef expression;
		int line;
};
}
}

#endif // ES_STATEMENT_H
