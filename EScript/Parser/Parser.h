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

#include <vector>

#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

namespace EScript {

class Block;
class Statement;

/*! [Parser] ---|> [Object] */
class Parser : public Object {
		ES_PROVIDES_TYPE_NAME(Parser)
	public:
		static Type* typeObject;
		static void init(EScript::Namespace & globals);

		/*! [Parser::Error] ---|> [Exception] ---|> [Object]    */
		class Error:public Exception {
			public:
				Error(const std::string  & _msg,Token * token=NULL):
					Exception(
						std::string("[Parser]")+_msg,  (token==NULL? -1 : token->getLine())) {
				}
				Error(const std::string  & _msg,const _CountedRef<Token> & token):
					Exception(
						std::string("[Parser]")+_msg,  (token.isNull() ? -1 : token->getLine())) {
				}
		};
		//-----------

		Parser(Type * type=NULL);
		virtual ~Parser();

		Object * parse(Block * rootBlock,const StringData & code) throw (Exception *);
		Object * parseFile(Block * rootBlock,const std::string & filename) throw (Exception *);

		/// ---|> [Object]
		virtual Object * clone()const;

		//! (internal)
		struct ParsingContext{
			Tokenizer::tokenList_t & tokens;
			Block * rootBlock;
			std::deque<Block*> blocks; // used as a stack
			ERef<String> code;
			ParsingContext(Tokenizer::tokenList_t & _tokens,const EPtr<String> & _code ) : tokens(_tokens),rootBlock(NULL),code(_code){}
		};

	private:
		// only for debugging
		identifierId currentFilename;
		std::string getCurrentFilename()const	{	return identifierIdToString(currentFilename);	}

		Tokenizer tokenizer;
		void pass_1(ParsingContext & ctxt)throw (Exception *);
		void pass_2(ParsingContext & ctxt, Tokenizer::tokenList_t  & enrichedTokens)const throw (Exception *);
		Statement getControl(ParsingContext & ctxt,int & cursor)const throw (Exception *);
		Statement getStatement(ParsingContext & ctxt,int & cursor,int to=-1)const throw (Exception *);
		Object * getExpression(ParsingContext & ctxt,int & cursor,int to=-1)const throw (Exception *);
		Object * getBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const throw (Exception *);
		Block * getBlock(ParsingContext & ctxt,int & cursor)const throw (Exception *) ;
		Object * getMap(ParsingContext & ctxt,int & cursor)const throw (Exception *);
		Object * getFunctionDeclaration(ParsingContext & ctxt,int & cursor)const throw (Exception *);
		UserFunction::parameterList_t * getFunctionParameters(ParsingContext & ctxt,int & cursor)const throw (Exception *);
		void getExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const throw (Exception *);

		Statement createStatement(Object * obj)const;

		enum lValue_t { LVALUE_NONE, LVALUE_INDEX, LVALUE_MEMBER};
		lValue_t getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,identifierId & identifier,Object * &indexExpression)const throw (Exception *);
		int findExpression(ParsingContext & ctxt,int cursor)const;
};
}

#endif // PARSER_H
