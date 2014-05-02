// Parser.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Tokenizer.h"

#include "../Objects/Exception.h"
#include "AST/UserFunctionExpr.h"
#include "../Utils/Logger.h"

#include <vector>

#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

namespace EScript {
namespace AST{
class Block;
}
struct LValueInfo;

//! [Parser]
class Parser {
	public:
		//! [ParserException] ---|> [Exception] ---|> [Object]
		class ParserException:public Exception {
			ES_PROVIDES_TYPE_NAME(ParserException)

			public:
				explicit ParserException(const std::string  & _msg,Token * token = nullptr):
					Exception(_msg,  (token==nullptr? -1 : token->getLine())) {
				}
				explicit ParserException(const std::string  & _msg,const _CountedRef<Token> & token):
					Exception(_msg,  (token.isNull() ? -1 : token->getLine())) {
				}
		};
		//-----------

		Parser(Logger * logger = nullptr);

		ERef<AST::Block> parse(const CodeFragment & code);

		//! (internal)
		struct ParsingContext{
			Tokenizer::tokenList_t & tokens;
			AST::Block * rootBlock;
			std::deque<AST::Block*> blocks; // used as a stack
			CodeFragment code;
			Logger& logger;
			ParsingContext(Tokenizer::tokenList_t & _tokens,const CodeFragment & _code,Logger& _logger ) : 
				tokens(_tokens),rootBlock(nullptr),code(_code),logger(_logger){}
		};

	private:

		_CountedRef<Logger> logger;

		void pass_1(ParsingContext & ctxt);
		void pass_2(ParsingContext & ctxt, Tokenizer::tokenList_t  & enrichedTokens)const;
		EPtr<AST::ASTNode> createAssignmentExpr(ParsingContext & ctxt,const LValueInfo& lValue,AST::ASTNode* rightExpression,int currentLine,int cursor)const;
		EPtr<AST::ASTNode> readAnnotatedStatement(ParsingContext & ctxt,int & cursor)const;
		EPtr<AST::ASTNode> readControl(ParsingContext & ctxt,int & cursor)const;
		EPtr<AST::ASTNode> readStatement(ParsingContext & ctxt,int & cursor)const;
		EPtr<AST::ASTNode> readExpression(ParsingContext & ctxt,int & cursor,int to=-1)const;
		EPtr<AST::ASTNode> readBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const;
		AST::Block * readBlockExpression(ParsingContext & ctxt,int & cursor)const ;
		EPtr<AST::ASTNode> readMap(ParsingContext & ctxt,int & cursor)const;
		EPtr<AST::ASTNode> readFunctionDeclaration(ParsingContext & ctxt,int & cursor)const;
		AST::UserFunctionExpr::parameterList_t readFunctionParameters(ParsingContext & ctxt,int & cursor)const;
		std::vector<ERef<AST::ASTNode>> readExpressionsInBrackets(ParsingContext & ctxt,int & cursor)const;

		typedef std::vector<std::pair<StringId,int> > annotations_t; //  (property's id, position of option bracket or -1)*
		annotations_t readAnnotation(ParsingContext & ctxt,int from,int to)const;

		LValueInfo getLValue(ParsingContext & ctxt,int from,int to)const;
		int findExpression(ParsingContext & ctxt,int cursor)const;

};
}

#endif // PARSER_H
