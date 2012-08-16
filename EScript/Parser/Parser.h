// Parser.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Tokenizer.h"

#include "../Objects/Exception.h"
#include "../Objects/Values/String.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Objects/AST/UserFunctionExpr.h"
#include "../Utils/Logger.h"

#include <vector>

#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

namespace EScript {
namespace AST{
class BlockExpr;
class Statement;
}

/*! [Parser] */
class Parser {
	public:

		/*! [ParserException] ---|> [Exception] ---|> [Object]    */
		class ParserException:public Exception {
			ES_PROVIDES_TYPE_NAME(ParserException)

			public:
				explicit ParserException(const std::string  & _msg,Token * token=NULL):
					Exception(_msg,  (token==NULL? -1 : token->getLine())) {
				}
				explicit ParserException(const std::string  & _msg,const _CountedRef<Token> & token):
					Exception(_msg,  (token.isNull() ? -1 : token->getLine())) {
				}
		};
		//-----------

		Parser(Logger * logger=NULL);
		~Parser();

		ERef<AST::BlockExpr> parse(const CodeFragment & code);

		//! (internal)
		struct ParsingContext{
			Tokenizer::tokenList_t & tokens;
			AST::BlockExpr * rootBlock;
			std::deque<AST::BlockExpr*> blocks; // used as a stack
			CodeFragment code;
			ParsingContext(Tokenizer::tokenList_t & _tokens,const CodeFragment & _code ) : tokens(_tokens),rootBlock(NULL),code(_code){}
		};

	private:

		_CountedRef<Logger> logger;
		void log(ParsingContext & ctxt,Logger::level_t messageLevel, const std::string & msg,const _CountedRef<Token> & token=NULL)const;

		Tokenizer tokenizer;
		void pass_1(ParsingContext & ctxt);
		void pass_2(ParsingContext & ctxt, Tokenizer::tokenList_t  & enrichedTokens)const;
		AST::Statement readControl(ParsingContext & ctxt,int & cursor)const;
		AST::Statement readStatement(ParsingContext & ctxt,int & cursor)const;
		Object * readExpression(ParsingContext & ctxt,int & cursor,int to=-1)const;
		Object * readBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const;
		AST::BlockExpr * readBlock(ParsingContext & ctxt,int & cursor)const ;
		Object * readMap(ParsingContext & ctxt,int & cursor)const;
		Object * readFunctionDeclaration(ParsingContext & ctxt,int & cursor)const;
		void readFunctionParameters(UserFunctionExpr::parameterList_t & params,ParsingContext & ctxt,int & cursor)const;
		void readExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const;

		typedef std::vector<std::pair<StringId,int> > properties_t; //  (property's id, position of option bracket or -1)*
		void readProperties(ParsingContext & ctxt,int from,int to,properties_t & properties)const;

		enum lValue_t { LVALUE_NONE, LVALUE_INDEX, LVALUE_MEMBER};
		lValue_t getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,StringId & identifier,Object * &indexExpression)const;
		int findExpression(ParsingContext & ctxt,int cursor)const;

		void throwError(ParsingContext & ctxt,const std::string & msg,Token * token=NULL)const;
		void throwError(ParsingContext & ctxt,const std::string & msg,const _CountedRef<Token> & token)const	{	throwError(ctxt,msg,token.get());	}
};
}

#endif // PARSER_H
