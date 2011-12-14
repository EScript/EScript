// Statement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STATEMENT_H
#define STATEMENT_H

#include "../Object.h"
#include "../../Utils/ObjRef.h"

namespace EScript {

/*! [Statement]  */
class Statement {
	public:
		enum type_t{
			TYPE_EXPRESSION,
			TYPE_BLOCK,
			TYPE_IF,
			TYPE_BREAK,
			TYPE_CONTINUE,
			TYPE_RETURN,
			TYPE_YIELD,
			TYPE_JUMP_TO_A,

			// less frequent
			TYPE_EXCEPTION,
			TYPE_EXIT,
			TYPE_UNDEFINED
		};

		Statement( const Statement & other);
		Statement( type_t _type, ObjPtr _expression);
		Statement( type_t _type=TYPE_UNDEFINED );

		~Statement();

		type_t getType()const			{	return type;	}
		Object* getExpression()const	{	return expression.get();	}

		int getLine()const				{	return line;	}
		void setLine(int newLine)		{	line=newLine;	}

		bool isValid()const				{	return type!=TYPE_UNDEFINED;	}
		bool operator==(const Statement & other){
			return type==other.type && expression==other.expression && line==other.line;
		}
		Statement & operator=(const Statement & other){
			if(this!=&other){
				type=other.type;
				expression=other.expression;
				line=other.line;
			}
			return *this;
		}

	private:
		type_t type;
		ObjRef expression;
		int line;
};
}

#endif // STATEMENT_H
