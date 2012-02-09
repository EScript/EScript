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
		static Type * getTypeObject();
		static void init(EScript::Namespace & globals);

		/*! [ParserException] ---|> [Exception] ---|> [Object]    */
		class ParserException:public Exception {
			ES_PROVIDES_TYPE_NAME(ParserException)

			public:
				ParserException(const std::string  & _msg,Token * token=NULL):
					Exception(_msg,  (token==NULL? -1 : token->getLine())) {
				}
				ParserException(const std::string  & _msg,const _CountedRef<Token> & token):
					Exception(_msg,  (token.isNull() ? -1 : token->getLine())) {
				}
		};
		//-----------

		Parser(Type * type=NULL);
		virtual ~Parser();

		Object * parse(Block * rootBlock,const StringData & code);
		Block * parseFile(const std::string & filename);

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

		enum warningLevel_t{
			NO_WARNINGS = 100,
			DEFAULT_WARNING = 10,
			PEDANTIC_WARNING = 5,
			DEBUG_INFO = 0
		};

	private:
		void info(warningLevel_t messageLevel, const std::string & msg,const _CountedRef<Token> & token=NULL)const;
		
		warningLevel_t warningLevel;
		
		// only for debugging
		identifierId currentFilename;
		std::string getCurrentFilename()const	{	return identifierIdToString(currentFilename);	}

		Tokenizer tokenizer;
		void pass_1(ParsingContext & ctxt);
		void pass_2(ParsingContext & ctxt, Tokenizer::tokenList_t  & enrichedTokens)const;
		Statement getControl(ParsingContext & ctxt,int & cursor)const;
		Statement getStatement(ParsingContext & ctxt,int & cursor,int to=-1)const;
		Object * getExpression(ParsingContext & ctxt,int & cursor,int to=-1)const;
		Object * getBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const;
		Block * getBlock(ParsingContext & ctxt,int & cursor)const ;
		Object * getMap(ParsingContext & ctxt,int & cursor)const;
		Object * getFunctionDeclaration(ParsingContext & ctxt,int & cursor)const;
		UserFunction::parameterList_t * getFunctionParameters(ParsingContext & ctxt,int & cursor)const;
		void getExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const;
	
		typedef std::vector<std::pair<identifierId,int> > annotations_t; //  (annotation's id, position of option bracket or -1)*
		void getAnnotations(ParsingContext & ctxt,int from,int to,annotations_t & annotations)const;

		Statement createStatement(Object * obj)const;

		enum lValue_t { LVALUE_NONE, LVALUE_INDEX, LVALUE_MEMBER};
		lValue_t getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,identifierId & identifier,Object * &indexExpression)const;
		int findExpression(ParsingContext & ctxt,int cursor)const;

		void throwError(const std::string & msg,Token * token=NULL)const;
		void throwError(const std::string & msg,const _CountedRef<Token> & token)const	{	throwError(msg,token.get());	}
};
}

#endif // PARSER_H
