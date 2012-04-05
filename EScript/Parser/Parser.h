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
class BlockStatement;
class Statement;
}

/*! [Parser] ---|> [Object] */
class Parser : public Object {
		ES_PROVIDES_TYPE_NAME(Parser)
	public:
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);

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

		bool _produceBytecode;

		Parser(Logger * logger=NULL, Type * type=NULL);
		virtual ~Parser();

		Object * parse(AST::BlockStatement * rootBlock,const StringData & code);
		AST::BlockStatement * parseFile(const std::string & filename);

		/// ---|> [Object]
		virtual Object * clone()const;

		//! (internal)
		struct ParsingContext{
			Tokenizer::tokenList_t & tokens;
			AST::BlockStatement * rootBlock;
			std::deque<AST::BlockStatement*> blocks; // used as a stack
			ERef<String> code;
			ParsingContext(Tokenizer::tokenList_t & _tokens,const EPtr<String> & _code ) : tokens(_tokens),rootBlock(NULL),code(_code){}
		};

	private:
		_CountedRef<Logger> logger;
		void log(Logger::level_t messageLevel, const std::string & msg,const _CountedRef<Token> & token=NULL)const;

		// only for debugging
		StringId currentFilename;
		std::string getCurrentFilename()const	{	return currentFilename.toString();	}

		Tokenizer tokenizer;
		void pass_1(ParsingContext & ctxt);
		void pass_2(ParsingContext & ctxt, Tokenizer::tokenList_t  & enrichedTokens)const;
		AST::Statement readControl(ParsingContext & ctxt,int & cursor)const;
		AST::Statement readStatement(ParsingContext & ctxt,int & cursor,int to=-1)const;
		Object * readExpression(ParsingContext & ctxt,int & cursor,int to=-1)const;
		Object * readBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const;
		AST::BlockStatement * readBlock(ParsingContext & ctxt,int & cursor)const ;
		Object * readMap(ParsingContext & ctxt,int & cursor)const;
		Object * readFunctionDeclaration(ParsingContext & ctxt,int & cursor)const;
		void readFunctionParameters(UserFunctionExpr::parameterList_t & params,ParsingContext & ctxt,int & cursor)const;
		void readExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const;

		typedef std::vector<std::pair<StringId,int> > properties_t; //  (property's id, position of option bracket or -1)*
		void readProperties(ParsingContext & ctxt,int from,int to,properties_t & properties)const;

		enum lValue_t { LVALUE_NONE, LVALUE_INDEX, LVALUE_MEMBER};
		lValue_t getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,StringId & identifier,Object * &indexExpression)const;
		int findExpression(ParsingContext & ctxt,int cursor)const;

		void throwError(const std::string & msg,Token * token=NULL)const;
		void throwError(const std::string & msg,const _CountedRef<Token> & token)const	{	throwError(msg,token.get());	}
};
}

#endif // PARSER_H
