// Parser.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Parser.h"
#include "Tokenizer.h"
#include "Operators.h"
#include "AST/AnnotatedStatement.h"
#include "AST/ControlStatements.h"
#include "AST/FunctionCallExpr.h"
#include "AST/GetAttributeExpr.h"
#include "AST/SetAttributeExpr.h"
#include "AST/IfStatement.h"
#include "AST/ConditionalExpr.h"
#include "AST/LogicOpExpr.h"
#include "AST/LoopStatement.h"
#include "AST/SwitchCaseStatement.h"
#include "AST/TryCatchStatement.h"
#include "AST/ValueExpr.h"
#include "../Consts.h"

#include "../Utils/IO/IO.h"

#include <stdio.h>
#include <stack>
#include <sstream>

namespace EScript {
using namespace AST;

// -------------------------------------------------------------------------------------------------------------------
// helper

template<class BracketStart,class BracketEnd>
int findCorrespondingBracket(const Parser::ParsingContext & ctxt,int from,int to=-1,int direction = 1){
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if(!Token::isA<BracketStart>(tokens.at(from))){
		std::cerr << "Unkwown error in brackets (should not happen!)\n";
		return -1;
	}
	int cursor = from;
	int level = 1;
	while(cursor != to){
		cursor+=direction;
		if(Token::isA<BracketStart>(tokens.at(cursor))){
			++level;
		}else if(Token::isA<BracketEnd>(tokens.at(cursor))){
			--level;
		}else if(Token::isA<TEndScript>(tokens.at(cursor))){
			return -1;
		}
		if(level==0)
			return cursor;
	}
	return -1;
}


// -------------------------------------------------------------------------------------------------------------------

//!	(ctor)
Parser::Parser(Logger * _logger) :
		logger(_logger ? _logger : new StdLogger(std::cout)) {
	//ctor
}

void Parser::log(ParsingContext & ctxt,Logger::level_t messageLevel, const std::string & msg,const _CountedRef<Token> & token)const{
	std::ostringstream os;
	os << "[Parser] " << msg << " (" << ctxt.code.getFilename();
	if(token!=nullptr)
		os << ':' << token->getLine();
	os << ").";
	logger->log(messageLevel,os.str());
}

ERef<AST::Block> Parser::parse(const CodeFragment & code) {
	ERef<AST::Block> rootBlock = AST::Block::createBlockExpression();

	tokenizer.defineToken("__FILE__",new TValueString(code.getFilename()));
	tokenizer.defineToken("__DIR__",new TValueString(IO::dirname(code.getFilename())));

	Tokenizer::tokenList_t tokens;
	ParsingContext ctxt(tokens,code);
	ctxt.rootBlock = rootBlock.get();

	/// 1. Tokenize
	try {
		tokenizer.getTokens(code.getCodeString(),tokens); //! \todo Use codeFragment for Tokenizer
		pass_1(ctxt);
	} catch (Exception * e) {
		//std::cerr << e->toString() << std::endl;
		throw;
	}
	/// 2. Parse definitions
	{
		Tokenizer::tokenList_t  enrichedTokens;
		pass_2(ctxt,enrichedTokens);
		tokens.swap(enrichedTokens);
	}

	/// 3. Parse expressions
	int cursor = 0;
	readStatement(ctxt,cursor);

	return rootBlock;
}

//! [Helper]
struct _BracketInfo {
	Token * token;
	unsigned int index;
	bool isBlockOrMap, containsColon, containsCommands,
		nextCBlockIsSwitchCaseBlock,isSwitchCaseBlock;
	int shortIf; // a?b:c

	_BracketInfo(unsigned int _index = 0,Token * _token = nullptr):
			token(_token),index(_index),
			isBlockOrMap(false),
			containsColon(false),containsCommands(false),
			nextCBlockIsSwitchCaseBlock(false),isSwitchCaseBlock(false),
			shortIf(0) {};
	_BracketInfo(const _BracketInfo & b) = default;

};
/**
 * Pass 1
 * =========
 * - check Syntax of Brackets () [] {}
 * - disambiguate Map/Block
 * - colon ( Mapdelimiter / shortIf ?:)
 */
void Parser::pass_1(ParsingContext & ctxt) {
	Tokenizer::tokenList_t & tokens = ctxt.tokens;

	std::stack<_BracketInfo> bInfStack;
	bInfStack.push(_BracketInfo());

	for(size_t cursor = 0;cursor<tokens.size();++cursor) {
		Token * token = tokens.at(cursor).get();
		/// currentBlockInfo
		_BracketInfo & cbi = bInfStack.top();
		switch(token->getType()){
			case TStartBracket::TYPE_ID:{
				bInfStack.push(_BracketInfo(cursor,token));
				continue;
			}
			case TStartBlock::TYPE_ID:{
				if(bInfStack.top().nextCBlockIsSwitchCaseBlock){
					bInfStack.top().nextCBlockIsSwitchCaseBlock = false;
					bInfStack.push(_BracketInfo(cursor,token));
					bInfStack.top().isSwitchCaseBlock = true;
				}else{
					bInfStack.push(_BracketInfo(cursor,token));
				}
				bInfStack.top().isBlockOrMap = true; // isBlockOrMap
				continue;
			}
			case TStartIndex::TYPE_ID:{
				bInfStack.push(_BracketInfo(cursor,token));
				continue;
			}
			case TEndBracket::TYPE_ID:{
				if(!Token::isA<TStartBracket>(cbi.token)) {
					throwError(ctxt,"Syntax error: ')'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndIndex::TYPE_ID:{
				if(!Token::isA<TStartIndex>(cbi.token)) {
					throwError(ctxt,"Syntax error: ']'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndBlock::TYPE_ID:{
				if(!cbi.isBlockOrMap) {
					throwError(ctxt,"Syntax error: '}'",token);
				}
				/// Block is Map Constructor
				if( cbi.containsColon && !cbi.isSwitchCaseBlock) {
					unsigned int startIndex = cbi.index;
					Token * t = new TStartMap;
					if( !tokens.at(startIndex).isNull() )
						t->setLine((tokens.at(startIndex))->getLine());

					tokens[startIndex] = t;
					t = new TEndMap;
					if( !tokens.at(cursor).isNull() )
						t->setLine((tokens.at(cursor))->getLine());

					tokens[cursor]=t;
				}
				bInfStack.pop();
				continue;
			}
			case TControl::TYPE_ID:{
				cbi.containsCommands = true;
				if(Token::cast<TControl>(token)->getId() == Consts::IDENTIFIER_switch){
					cbi.nextCBlockIsSwitchCaseBlock = true;
				}
				continue;
			}
			default:{
			}
		}
		if(cbi.isBlockOrMap) {
			if(Token::isA<TColon>(token) ){
				if(cbi.shortIf>0) {
					cbi.shortIf--;
				} else if(cbi.isSwitchCaseBlock){
					/// ok
				} else if(cbi.containsCommands) {
					throwError(ctxt,"Syntax error in Block: ':'",token);
				} else { /// block is a map constructor
					cbi.containsColon = true;
					Token * t = new TMapDelimiter;

					if((tokens.at(cursor))!=nullptr)
						t->setLine((tokens.at(cursor).get())->getLine());
//					Token::removeReference(tokens.at(cursor));

					tokens[cursor]=t;
//					Token::addReference(t);
					continue;
				}
			} else if(Token::isA<TEndCommand>(token)) {
				if(cbi.containsColon) {
					throwError(ctxt,"Syntax error in Map: ';'",token);
				}
				cbi.containsCommands = true;
				cbi.shortIf = 0;
			} else if(Token::isA<TOperator>(token) && token->toString()=="?") {
				++cbi.shortIf;
			}


		}
	}
	//std::cout << "\n###"<<tStack.top()->toString();
	if(bInfStack.top().token!=nullptr) {
		throwError(ctxt,"Unexpected eof (unclosed '"+bInfStack.top().token->toString()+"'?)",bInfStack.top().token);
	}
}

/**
 * Pass 2
 * =========
 * - Create Block-Objects
 * - Parse declarations (var)
 * - Wrap parts of "fn" in brackets for easier processing: fn(foo,bar){...}  --->  fn( (foo,bar){} )
 * - Change loop brackets to blocks to handle loop wide scope handling: while(...) ---> while{...}
 * - TODO: Class declaration
 * ?????- TODO: Undefined scope for i: "{ var i;  do{ var i; }while(var i); }"
 */
void Parser::pass_2(ParsingContext & ctxt,
					Tokenizer::tokenList_t & enrichedTokens)const  {

	std::stack<Block *> blockStack;
	blockStack.push(ctxt.rootBlock);

	/// Counts the currently open brackets and blocks for the current function declaration.
	/// If the top value reaches 0 after reading a TEndBlock, the fn-wrapper brackets have to be closed.
	std::stack<int> functionBracketDepth;

	std::stack<TStartBracket*> currentBracket;

	/// for(...) ---> for{...}
	std::stack<Token*> loopConditionEndingBrackets;

	enrichedTokens.reserve(ctxt.tokens.size());

	TStartBlock * tsb = new TStartBlock(ctxt.rootBlock);
//	Token::addReference(tsb);
	enrichedTokens.push_back(tsb);

	for(size_t cursor = 0;cursor<ctxt.tokens.size();++cursor) {
		Token * token = ctxt.tokens.at(cursor).get();

		/// for(...) ---> for{...}
		if(!loopConditionEndingBrackets.empty() && token == loopConditionEndingBrackets.top()){
			loopConditionEndingBrackets.pop();
			Token * t = new TEndBlock;
			t->setLine(token->getLine());
//			Token::addReference(t);
			token = t;
		}


		switch(token->getType()){
			case TControl::TYPE_ID:{
				TControl * tc = Token::cast<TControl>(token);
				/// Variable Declaration
				if(tc->getId()==Consts::IDENTIFIER_var) {
					if(TIdentifier * ti = Token::cast<TIdentifier>(ctxt.tokens.at(cursor+1))) {
						if(!blockStack.top()->declareLocalVar(ti->getId())){
							log(ctxt,Logger::LOG_WARNING, "Duplicate local variable '"+ti->toString()+'\'',ti);
						}
						continue;
					} else
						throwError(ctxt,"var expects identifier.",tc);
				}
				/// Static variable Declaration
				else if(tc->getId()==Consts::IDENTIFIER_static) {
					if(TIdentifier * ti = Token::cast<TIdentifier>(ctxt.tokens.at(cursor+1))) {
						if(!blockStack.top()->declareStaticVar(ti->getId())){
							log(ctxt,Logger::LOG_WARNING, "Duplicate static variable '"+ti->toString()+'\'',ti);
						}
						continue;
					} else
						throwError(ctxt,"static expects identifier.",tc);
				}
				/// for(...) ---> for{...}
				else if(tc->getId()==Consts::IDENTIFIER_for || tc->getId()==Consts::IDENTIFIER_foreach || tc->getId()==Consts::IDENTIFIER_while){
					if( ctxt.tokens.at(cursor+1)->getType()!=TStartBracket::TYPE_ID )
						throwError(ctxt,tc->toString()+" expects '('",tc);
					int endPos = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor+1);
					if(endPos<0)
						throwError(ctxt,"Error in loop condition",tc);
					loopConditionEndingBrackets.push(ctxt.tokens.at(endPos).get());

					enrichedTokens.push_back(token);
					++cursor;
					Block * loopConditionBlock = Block::createBlockStatement(tc->getLine());
					blockStack.push(loopConditionBlock);

					TStartBlock * sb = new TStartBlock(loopConditionBlock);
					sb->setLine(token->getLine());
//					Token::addReference(sb);
					enrichedTokens.push_back(sb);

					// count open bracket.
					if(!functionBracketDepth.empty())
						++functionBracketDepth.top();
					continue;
				}
				enrichedTokens.push_back(token);
				continue;
				/// name='static' ??????
			}
			/// Open new Block
			case TStartBlock::TYPE_ID:{
				TStartBlock * sb = Token::cast<TStartBlock>(token);

				Block * currentBlock = Block::createBlockExpression(sb->getLine());

				blockStack.push(currentBlock);
				sb->setBlock(currentBlock);
				enrichedTokens.push_back(token);

				if(!functionBracketDepth.empty())
					++functionBracketDepth.top();

				continue;
			}
			/// Close Block
			case TEndBlock::TYPE_ID:{
				enrichedTokens.push_back(token);

				blockStack.pop();
				if(blockStack.empty())
					throwError(ctxt,"Unexpected }");

				if(!functionBracketDepth.empty()){
					--functionBracketDepth.top();

					if(functionBracketDepth.top()==0){
						functionBracketDepth.pop();
						Token * t = new TEndBracket;
						t->setLine(token->getLine());
						enrichedTokens.push_back(t);

						// add shortcut to the closing bracket
						currentBracket.top()->endBracketIndex = enrichedTokens.size()-1;
						currentBracket.pop();

						// second closing bracket
						t = new TEndBracket;
						t->setLine(token->getLine());
						enrichedTokens.push_back(t);

						// add shortcut to the closing bracket
						currentBracket.top()->endBracketIndex = enrichedTokens.size()-1;
						currentBracket.pop();
					}
				}
				continue;
			}
			/// (
			case TStartBracket::TYPE_ID:{
				enrichedTokens.push_back(token);
				if(!functionBracketDepth.empty())
					++functionBracketDepth.top();
				currentBracket.push(Token::cast<TStartBracket>(token));
				continue;
			}
			/// )
			case TEndBracket::TYPE_ID:{
				enrichedTokens.push_back(token);
				if(!functionBracketDepth.empty())
					--functionBracketDepth.top();
				if(currentBracket.empty())
					throwError(ctxt,"Missing opening bracket for ",token);

				// add shortcut to the closing bracket
				currentBracket.top()->endBracketIndex = enrichedTokens.size()-1;
				currentBracket.pop();
				continue;
			}
			// "part1" "part2"
			case TValueString::TYPE_ID:{
				enrichedTokens.push_back(token);

				// no consecutive strings?
				if( ctxt.tokens.at(cursor+1)->getType()!=TValueString::TYPE_ID ){
					continue;
				}
				std::stringstream os;

				TValueString * ts;
				while( (ts = Token::cast<TValueString>(ctxt.tokens.at(cursor)))!=nullptr ){
					os << ts->getValue();
					++cursor;
				}
				--cursor;

				Token::cast<TValueString>(token)->setString(os.str());
				continue;
			}
			/// fn(foo,bar){...}  ---> (fn( (foo,bar){} ))
			case TOperator::TYPE_ID:{
				if( token->toString() == "fn"  ) {
					functionBracketDepth.push(0);

					// bracket before 'fn'
					TStartBracket * t = new TStartBracket;
					t->setLine(token->getLine());
					currentBracket.push(t);
					enrichedTokens.push_back(t);

					enrichedTokens.push_back(token);

					// bracket after 'fn'
					t = new TStartBracket;
					t->setLine(token->getLine());
					currentBracket.push(t);
					enrichedTokens.push_back(t);
				}else{
					enrichedTokens.push_back(token);
				}
				continue;
			}
			/// End of script
			case TEndScript::TYPE_ID:{
				blockStack.pop();
				if(!blockStack.empty())
					throwError(ctxt,"Unclosed {");

				Token * t = new TEndBlock;
				t->setLine(token->getLine());
				enrichedTokens.push_back(t);
				enrichedTokens.push_back(token);
				return;
			}
			/// ...
			default:{
				enrichedTokens.push_back(token);
			}
		}
	}
}

/*! read an expression
 * \note @p cursor is moved to @p to, or an exception is thrown.
 */
EPtr<AST::ASTNode> Parser::readExpression(ParsingContext & ctxt,int & cursor,int to)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if(cursor>=static_cast<int>(tokens.size())){
		return nullptr;
	}/// Commands: if(...){}
	else if(Token::isA<TControl>(tokens.at(cursor))) {
		log(ctxt,Logger::LOG_WARNING, "No control statement here!",tokens.at(cursor));
		return nullptr;
	} /// Block: {...}
	else if(Token::isA<TStartBlock>(tokens.at(cursor))) {
		return readBlockExpression(ctxt,cursor);
	}

	/// If "to" is not given, search the end of the expression
	if(to==-1) {
		to = findExpression(ctxt,cursor);
	}

	/// Only happens when searching for non existing Expression:
	///  the empty side of binary Expression (empty)!a or a++(empty)
	if(to<cursor) {
		return nullptr;
	}

	///  Single Element
	/// -------------------
	else if(to==cursor) {
		Token *t =tokens.at(cursor).get();

		/// Empty Command
		if(Token::isA<TEndCommand>(t)) {
			return nullptr;
		}else if(TValueBool * tb = Token::cast<TValueBool>(t)) {
			return new BoolValueExpr(tb->getValue());
		}else if(TValueNumber * tn = Token::cast<TValueNumber>(t)) {
			return new NumberValueExpr(tn->getValue());
		}else if(TValueIdentifier * ti = Token::cast<TValueIdentifier>(t)) {
			return new IdentifierValueExpr(ti->getValue());
		}else if(TValueString * ts = Token::cast<TValueString>(t)) {
			return new StringValueExpr(ts->getValue());
		}else if(Token::isA<TValueVoid>(t)) {
			return new VoidValueExpr;
		}
		///  Identifier
		/// "a" => "_.get('a')"
		else if(TIdentifier * ident = Token::cast<TIdentifier>(t)) {
//			// is local variable?
//			for(int i = ctxt.blocks.size()-1;i>=0;--i){
//				Block * b = ctxt.blocks.at(i);
//				if(b==nullptr)
//					break;
//				 else if(b->isLocalVar(ident->getId())){
////////					std::cout <<"local:"<<ident->toString()<<"\n";
//					break;
//				 }
//			}
			return new GetAttributeExpr(nullptr,ident->getId());  // ID
		}
		throwError(ctxt,"Unknown (or unimplemented) Token",t);
	}

	///  Command ends with ;
	///  "2;"
	/// ---------------------
	else if(Token::isA<TEndCommand>(tokens[to])) {
		EPtr<AST::ASTNode> e = readExpression(ctxt,cursor,to-1);
		++cursor;
		return e;
	}

	/// Surrounded with Brackets
	/// "(a+2)"
	/// --------------------------
	else if(Token::isA<TStartBracket>(tokens.at(cursor)) &&
			 Token::isA<TEndBracket>(tokens[to]) &&
			 findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to,1)==to) {

		++cursor; // step over '('
		EPtr<AST::ASTNode> innerExpression = readExpression(ctxt,cursor,to-1);
		if(innerExpression.isNotNull()) // if expression is not empty
			++cursor; // step to ')'
		return innerExpression;
	}

	/// Map Constructor
	/// "{foo:bar,2:3}"
	/// --------------------------
	if(Token::isA<TStartMap>(tokens.at(cursor)) &&
		Token::isA<TEndMap>(tokens[to]) &&
		findCorrespondingBracket<TStartMap,TEndMap>(ctxt,cursor,to,1) == to) {
		return readMap(ctxt,cursor);
	}

	/// BinaryExpression
	/// "3+foo"
	/// --------------------------
	EPtr<AST::ASTNode> obj = readBinaryExpression(ctxt,cursor,to);
	if(obj.isNotNull()) {
		if(cursor!=to){
			throwError(ctxt,"Syntax error in expression.",tokens.at(cursor));
		}
		return obj;
	}

	///    Syntax Error
	/// --------------------
	else {
		throwError(ctxt,"Syntax error.",tokens.at(cursor));
		return nullptr;
	}
}

//! (internal)
EPtr<AST::ASTNode> Parser::readAnnotatedStatement(ParsingContext & ctxt,int & cursor)const{
	++cursor;
	const auto annotationStartBracket = ctxt.tokens.at(cursor);
	if(!Token::isA<TStartBracket>(annotationStartBracket)){
		throwError(ctxt,"Annotation expects brackets.",ctxt.tokens.at(cursor));
	}
	const int annotationTo = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor);
	const auto annotations = readAnnotation(ctxt,cursor+1,annotationTo-1);

	cursor = annotationTo+1;
	ERef<AST::ASTNode> statement = readStatement(ctxt,cursor);

	for(const auto & annotation : annotations) {
		const StringId & name = annotation.first;
		const int parameterPos = annotation.second;

		/*! \note if annotated statements support parameters, the compiler should be able
			to check the correct annotation usage. There would be no need to check the specific
			annotations here.
		*/
		if(name == Consts::ANNOTATION_STMT_once){
			if(parameterPos>=0)
				throwError(ctxt,"'once' annotation does not accept parameters",annotationStartBracket);
		}else{
			log(ctxt,Logger::LOG_WARNING,"Invalid statement annotation: '"+name.toString()+"'",annotationStartBracket);
			continue;
		}
		// wrap in annotator
		statement = new AnnotatedStatement(statement,name);
	}
	return statement.detachAndDecrease();
}

//! (internal)
EPtr<AST::ASTNode> Parser::readStatement(ParsingContext & ctxt,int & cursor)const{
	const auto & token = ctxt.tokens.at(cursor);
	if(Token::isA<TControl>(token)) {
		return readControl(ctxt,cursor);
	} /// sub-Block: {...}
	else if(Token::isA<TStartBlock>(token)) {
		Block * block = readBlockExpression(ctxt,cursor);
		block->convertToStatement();
		return block;
	} /// annotated statement
	else if(Token::isA<TOperator>(token) && token->toString()=="@") {
		return readAnnotatedStatement(ctxt,cursor);
	}/// expression
	else{
		return readExpression(ctxt,cursor);
	}

}

void Parser::assertTokenIsStatemetEnding(ParsingContext& ctxt,Token* token)const{
	/// Commands have to end on ";" or "}".
	if(!(Token::isA<TEndCommand>(token) || Token::isA<TEndBlock>(token))) {
		log(ctxt,Logger::LOG_DEBUG, token->toString(),token);
		throwError(ctxt,"Syntax error in Block (Missing ';' ?).",token);
	}
}

/*! Check for shadowed local variables.
	Used when reading blocks by readBlockExpression or when reading
	a case block.
	\note The issued warning has LOG_PEDANTIC_WARNING level.*/
void Parser::warnOnShadowedLocalVars(ParsingContext & ctxt,TStartBlock * tBlock)const{
	if(ctxt.blocks.empty())
		return;
	auto block = tBlock->getBlock();
	const auto & vars = block->getVars();
	if(vars.empty())
		return;
	for(int i = ctxt.blocks.size()-1; i>=0 && ctxt.blocks[i]!=nullptr; --i ){
		const declaredVariableMap_t & vars2 = ctxt.blocks[i]->getVars();
		if(vars2.empty())
			continue;
		for(const auto & var : vars) {
			if(vars2.count(var.first) > 0) {
				log(ctxt, Logger::LOG_PEDANTIC_WARNING, "Shadowed variable  '" + var.first.toString() + "' in block.", tBlock);
			}
		}
	}
}

/**
 * Get block of statements
 * {out("foo");exit;}
 * \note throws a syntax error if no Block can be read.
 */
Block * Parser::readBlockExpression(ParsingContext & ctxt,int & cursor)const {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TStartBlock * tsb = Token::cast<TStartBlock>(tokens.at(cursor));
	Block * b = tsb?reinterpret_cast<Block *>(tsb->getBlock()):nullptr;
	if(b==nullptr)
		throwError(ctxt,"No Block!",tokens.at(cursor));
	++cursor;

	warnOnShadowedLocalVars(ctxt,tsb);
	ctxt.blocks.push_back(b);

	/// Read commands.
	while(!Token::isA<TEndBlock>(tokens.at(cursor))) {
		if(Token::isA<TEndScript>(tokens.at(cursor)))
			throwError(ctxt,"Unclosed Block {...",tsb);

		const int line = tokens.at(cursor)->getLine();
		EPtr<AST::ASTNode> stmt = readStatement(ctxt,cursor);

		if(stmt.isNotNull()){
			b->addStatement(stmt);
			stmt->setLine(line);
		}
		assertTokenIsStatemetEnding(ctxt,tokens.at(cursor).get());
		++cursor;
	}
	ctxt.blocks.pop_back();
	return b;
}

//!	readMap
EPtr<AST::ASTNode> Parser::readMap(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if(!Token::isA<TStartMap>(tokens.at(cursor)))
		throwError(ctxt,"No Map!",tokens.at(cursor));

	// for debugging
	int currentLine=-1;
	{
		const Token * t = tokens.at(cursor).get();
		if(t)
			currentLine = t->getLine();
	}

	++cursor;

	EPtr<AST::ASTNode> exp;
	ASTNode::refArray_t paramExp;
	while(!Token::isA<TEndMap>(tokens.at(cursor))) {

		/// i) read Key

		/// Key is not present
		if(Token::isA<TMapDelimiter>(tokens.at(cursor))) {
			exp = new VoidValueExpr;
		} /// Key is present
		else {
			exp = readExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		/// ii) read ":"
		if(!Token::isA<TMapDelimiter>(tokens.at(cursor))) {
			log(ctxt,Logger::LOG_DEBUG, tokens.at(cursor)->toString(),tokens.at(cursor));
			throwError(ctxt,"Map: Expected : ",tokens.at(cursor));
		}
		++cursor;

		/// iii) read Value
		/// Value is not present (only valid for last tuple)
		if(Token::isA<TEndMap>(tokens.at(cursor))) {
			exp = new VoidValueExpr;
		} /// Value is present
		else {
			exp = readExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		if(Token::isA<TEndMap>(tokens.at(cursor)))
			break;
		else if(Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			continue;
		} else
			throwError(ctxt,"Syntax error in Map.",tokens.at(cursor));
	}

	return FunctionCallExpr::createSysCall(Consts::SYS_CALL_CREATE_MAP,paramExp,currentLine);

}

/*!	read binary expression
	\note called by readExpression
	\note If the syntax is correct, @p cursor equals @p to after returning.
			readExpression issues an SyntaxError otherwise.
*/
EPtr<AST::ASTNode> Parser::readBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	int currentLine = tokens.at(cursor).isNull() ? -1 : tokens.at(cursor)->getLine();

	int opPosition=-1; /// Position of operator with lowest precedence
	int opPrecedence=-1; /// Highest precedence
	const Operator * op = nullptr;

	/// search operator with lowest precedence

	int level = 0; /// BracketLevel ( ) [] {}
	for(int i = cursor;i<=to;++i) {
		Token * t = tokens.at(i).get();
		if(level==0) {
			TOperator * top = Token::cast<TOperator>(t);
			if(top &&
					(top->getAssociativity()==Operator::L?
					 top->getPrecedence() >= opPrecedence :
					 top->getPrecedence() > opPrecedence)) {
				opPrecedence = top->getPrecedence();
				opPosition = i;
				op = top->getOperator();
				currentLine = top->getLine();
			}
		}
		if(Token::isA<TStartBlock>(t)
				||Token::isA<TStartBracket>(t)
				||Token::isA<TStartIndex>(t)
				||Token::isA<TStartMap>(t)) {
			++level;
			continue;
		} else if(Token::isA<TEndBlock>(t)
				   ||Token::isA<TEndBracket>(t)
				   ||Token::isA<TEndIndex>(t)
				   ||Token::isA<TEndMap>(t)) {
			--level;
			if(level<0) {
				throwError(ctxt,"Error in binary expression",t);
			}
			continue;

		}
	}
	if(opPosition<0 || !op) return nullptr;

	int rightExprFrom = opPosition+1;
	int leftExprFrom = cursor,leftExprTo = opPosition-1;

	/// ASSIGNMENTS ( "="  ":=" )
	/// -----------
	if(op->getString()=="=") {
		StringId memberIdentifier;
		EPtr<AST::ASTNode> obj;
		EPtr<AST::ASTNode> indexExp;
		int lValueType = getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		ERef<ASTNode> rightExpression = readExpression(ctxt,rightExprFrom,to);
		cursor = rightExprFrom;


		/// a = 2 => _.[a] = 2
		if(lValueType== LVALUE_MEMBER) {
			return SetAttributeExpr::createAssignment(obj,memberIdentifier,rightExpression.get(),currentLine);
		}
		/// a[1]=2 =>  _.a._set(1, 2)
		else if(lValueType == LVALUE_INDEX) {
			ASTNode::refArray_t paramExp;
			paramExp.push_back(indexExp);
			paramExp.push_back(rightExpression);
			return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(obj,Consts::IDENTIFIER_fn_set),paramExp,currentLine);
		} else {
//			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throwError(ctxt,"No valid LValue before '=' ",tokens[opPosition]);
		}
	} else if(op->getString()==":=" || op->getString()=="::=") {
		Attribute::flag_t flags = op->getString()=="::=" ? Attribute::TYPE_ATTR_BIT : 0;
		Attribute::flag_t inverseFlags = 0;

		/// extract annotations:  Object.member @(annotation*) := value
		///                       leftExpr      annotation        rightExpr
		if( Token::isA<TEndBracket>(tokens[leftExprTo]) ){
			int annotationStart = findCorrespondingBracket<TEndBracket,TStartBracket>(ctxt,leftExprTo,leftExprFrom,-1);
			TOperator * atOp = Token::cast<TOperator>(tokens.at(annotationStart-1));
			if(annotationStart>0 && atOp!=nullptr && atOp->toString()=="@"){
				const auto annotations = readAnnotation(ctxt,annotationStart+1,leftExprTo-1);
				leftExprTo = annotationStart-2;

				for(const auto & annotation : annotations) {
					const StringId & name = annotation.first;
					log(ctxt,Logger::LOG_INFO,"Annotation:"+name.toString(),atOp );
					if(name == Consts::ANNOTATION_ATTR_const){
						flags |= Attribute::CONST_BIT;
					}else if(name == Consts::ANNOTATION_ATTR_init){
						if(flags&Attribute::TYPE_ATTR_BIT)
							log(ctxt,Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
						flags |= Attribute::INIT_BIT;
					}else if(name == Consts::ANNOTATION_ATTR_member){
						if(flags&Attribute::TYPE_ATTR_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::TYPE_ATTR_BIT;
						}
					}else if(name == Consts::ANNOTATION_ATTR_override){
						flags |= Attribute::OVERRIDE_BIT;
					}else if(name == Consts::ANNOTATION_ATTR_private){
						if(inverseFlags&Attribute::PRIVATE_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(private)' is used in combination with @(public) and is ignored.",atOp);
						}else{
							flags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::ANNOTATION_ATTR_public){
						if(flags&Attribute::PRIVATE_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(public)' is used in combination with @(private) and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::ANNOTATION_ATTR_type){
						if(inverseFlags&Attribute::TYPE_ATTR_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							flags |= Attribute::TYPE_ATTR_BIT;
						}
						if(flags&Attribute::INIT_BIT)
							log(ctxt,Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
					}else {
						throwError(ctxt,"Invalid annotation: '"+name.toString()+'\'',atOp);
					}
				}
			}

		}
		StringId memberIdentifier;
		EPtr<AST::ASTNode> obj;

		EPtr<AST::ASTNode> indexExp;
		const int lValueType = getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		EPtr<AST::ASTNode> rightExpression = readExpression(ctxt,rightExprFrom,to);
		cursor = rightExprFrom;


		/// a:=2 => _.[a] := 2
		if(lValueType != LVALUE_MEMBER) {
			throwError(ctxt,"No valid member-LValue before '"+op->getString()+"' ",tokens[opPosition]);
		}
		if(obj==nullptr){
			log(ctxt,Logger::LOG_WARNING,"Use '=' for assigning to local variables instead of '"+op->getString()+"' ",tokens[opPosition]);
			return SetAttributeExpr::createAssignment(obj,memberIdentifier,rightExpression,currentLine);
		}
		return new SetAttributeExpr(obj,memberIdentifier,rightExpression,flags,currentLine);
	}


	/// get left expression
	EPtr<AST::ASTNode> leftExpression = readExpression(ctxt,leftExprFrom,leftExprTo);

	/// "a.b.c"
	if(op->getString()==".") {
		cursor = rightExprFrom;
		if(cursor>to) {
			log(ctxt,Logger::LOG_DEBUG, "Error .1 ",tokens[opPosition]);
			throwError(ctxt,"Syntax error after '.'.",tokens[opPosition]);
		}

		/// "a.b"
		if(Token::isA<TIdentifier>(tokens[cursor])){
			return new GetAttributeExpr(leftExpression,Token::cast<TIdentifier>(tokens[cursor])->getId());
		}
		/// "a.+"
		else if(Token::isA<TOperator>(tokens[cursor])) {
			return new GetAttributeExpr(leftExpression,Token::cast<TOperator>(tokens[cursor])->toString());
		} /// "a.'+'"
		else if(Token::isA<TValueString>(tokens[cursor])) {
			return new GetAttributeExpr(leftExpression,Token::cast<TValueString>(tokens[cursor])->getValue());
		}/// "a.$b"
		else if(Token::isA<TValueIdentifier>(tokens[cursor])){
			return new GetAttributeExpr(leftExpression,Token::cast<TValueIdentifier>(tokens[cursor])->getValue());
		}
		log(ctxt,Logger::LOG_DEBUG, "Error .2 ",tokens[opPosition]);
		throwError(ctxt,"Syntax error after '.'.",tokens[opPosition]);
	}
	///  Function Call
	/// "a(b)"  "a(1,2,3)"
	else if(op->getString()=="(") {
		cursor = rightExprFrom-1;
		ASTNode::refArray_t paramExps = readExpressionsInBrackets(ctxt,cursor);

		/// search for expanding parameters f(0,arr...,2,3)
		std::vector<uint32_t> expandingParams = extractExpandingParameters(paramExps);

		FunctionCallExpr * funcCall = FunctionCallExpr::createFunctionCall(leftExpression,paramExps,currentLine);
		funcCall->emplaceExpandingParameters(std::move(expandingParams));

		return funcCall;
	}
	///  Index Exression | Array
	else if(op->getString()=="[") {
		/// No left expression present? -> Array-constructor
		///"[1,a+2,3]" -> new Array(1,a+2,3)
		if(leftExpression.isNull()) {
			ASTNode::refArray_t paramExps;
			++cursor;
			while(!Token::isA<TEndIndex>(tokens.at(cursor)) ) {

				paramExps.push_back(readExpression(ctxt,cursor));

				++cursor;
				if(Token::isA<TDelimiter>(tokens.at(cursor)))
					++cursor;
				else if(!Token::isA<TEndIndex>(tokens.at(cursor))){
					throwError(ctxt,"Expected ]",tokens[opPosition]);
				}
			}
			/// search for expanding parameters f(0,arr...,2,3)
			auto expandingParams = extractExpandingParameters(paramExps);

			FunctionCallExpr * funcCall = FunctionCallExpr::createSysCall( Consts::SYS_CALL_CREATE_ARRAY,paramExps,currentLine);
			funcCall->emplaceExpandingParameters(std::move(expandingParams));
			return funcCall;
		}
		/// Left expression present? -> Index Expression
		/// "a[1]"
		cursor = rightExprFrom;
		ASTNode::refArray_t paramExps;
		paramExps.push_back(readExpression(ctxt,cursor));
		// cursor now points to ']'
		++cursor;
		return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(leftExpression,Consts::IDENTIFIER_fn_get),
										paramExps,currentLine);

	}/// "a?1:2"
	else if(op->getString()=="?") {
		cursor = rightExprFrom;
		EPtr<AST::ASTNode> alt1 = readExpression(ctxt,cursor);
		++cursor; // step to ':'
		if(!Token::isA<TColon>(tokens.at(cursor))) {
			throwError(ctxt,"Expected ':'",tokens.at(cursor));
		}
		++cursor;
		EPtr<AST::ASTNode> alt2 = readExpression(ctxt,cursor,to);
		return new ConditionalExpr(leftExpression,alt1,alt2);
	} /// new Object
	else if(op->getString()=="new") {
		++cursor;
		if(leftExpression.isNotNull())
			throwError(ctxt,"'new' is a unary left operator.",tokens.at(cursor));

		int objExprTo = to;

		/// if new has parameters "(...)", search for their beginning.
		if(Token::isA<TEndBracket>(tokens[to])) {
			const int leftBracket = findCorrespondingBracket<TEndBracket,TStartBracket>(ctxt,objExprTo,rightExprFrom,-1);
			if(leftBracket>cursor) /// if leftBracket==cursor the brackets enclose the objExpr and not the parameters. e.g. new (A);
				objExprTo = leftBracket;
		}
		/// read parameters
		ASTNode::refArray_t paramExp;
		std::vector<uint32_t> expandingParams;
		if(objExprTo<to) {
			int cursor2 = objExprTo;
			paramExp = readExpressionsInBrackets(ctxt,cursor2);
			expandingParams = std::move(extractExpandingParameters(paramExp));
			objExprTo--; /// step over '('
		}
		/// read Object-expression
		EPtr<AST::ASTNode> obj = readExpression(ctxt,cursor,objExprTo);
		if(obj.isNull())
			throwError(ctxt,"[new] Syntax error.",tokens.at(cursor));
		cursor = to; // set cursor at end of parameter list
		FunctionCallExpr * funcCall = FunctionCallExpr::createConstructorCall(obj,paramExp,currentLine);
		funcCall->emplaceExpandingParameters(std::move(expandingParams));
		return funcCall;
	}
	/// Function "fn(a,b){return a+b;}"
	else if(op->getString()=="fn" ){//|| op->getString()=="lambda") {
		ERef<ASTNode> result = readFunctionDeclaration(ctxt,cursor);
		if(cursor!=to)
			throwError(ctxt,"[fn] Syntax error.",tokens.at(cursor));
		return result.detachAndDecrease();
	}

	EPtr<AST::ASTNode> rightExpression = readExpression(ctxt,rightExprFrom,to);

	cursor = rightExprFrom;

	/// Unary prefix expression
	/// ++a, --a, !a
	/// Bsp.: ++a =>  _.a.++pre()
	if(leftExpression.isNull()) {
		/// +a  +3
		if(op->getString()=="+"){
			// @optimization
			if(NumberValueExpr* num = dynamic_cast<NumberValueExpr*>(rightExpression.get())) {
				return num;
			}
		}
		/// -a  -3
		else if(op->getString()=="-") {
			// @optimization
			if(NumberValueExpr* num = dynamic_cast<NumberValueExpr*>(rightExpression.get())) {
				num->setValue( -num->getValue() );
				return num;
			}
//            if(Number* num = dynamic_cast<Number*>(rightExpression)) {
//                Number * newNum = Number::create(-num->toDouble());
//                delete num;
//                return newNum ;
//            }
		} else if(op->getString()=="!") {
			return LogicOpExpr::createNot(rightExpression) ;
		}

		//if(GetAttributeExpr * ga = dynamic_cast<GetAttributeExpr *>(rightExpression)) {
		FunctionCallExpr * fc = FunctionCallExpr::createFunctionCall(
			new GetAttributeExpr(rightExpression,
							 std::string(op->getString())+"_pre"),ASTNode::refArray_t(),currentLine);
		return  fc;

	} else
		/// Unary postfix expression
		/// a++, a--, a!
		/// Bsp: a++ => _.a.++post()
		if(rightExpression.isNull()) {
			//  if(GetAttributeExpr * ga = dynamic_cast<GetAttributeExpr *>(leftExpression)) {
			FunctionCallExpr * fc = FunctionCallExpr::createFunctionCall(
				new GetAttributeExpr(leftExpression,
								 std::string(op->getString())+"_post"),ASTNode::refArray_t(),currentLine);
			cursor--;

			return  fc;
		}
	/// ||
		else if(op->getString()=="||") {
			return LogicOpExpr::createOr(leftExpression,rightExpression);
		}
	/// &&
		else if(op->getString()=="&&") {
			return LogicOpExpr::createAnd(leftExpression,rightExpression);
		}
	/// normal binary expression
	/// 1+2 -> 1.+(2)
		else {
			ASTNode::refArray_t paramExp;
			paramExp.push_back(rightExpression);
			return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(leftExpression, op->getString()),
													paramExp,currentLine);
		}
	return nullptr;
}

/*!	Read a function declaration. Must begin with "fn"
	Cursor is placed at the end of the block.
	\note after pass_2(...) a function looks like this:
			fn( (params*) {...} )  OR
			fn( (params*).(constrExpr) {...} )
			fn( (params*)@(super()) {...} )
	*/
EPtr<AST::ASTNode> Parser::readFunctionDeclaration(ParsingContext & ctxt,int & cursor)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t = tokens.at(cursor).get();

	if(t->toString()!="fn"){
		throwError(ctxt,"No function! ",tokens.at(cursor));
	}
	const size_t codeStartPos = t->getStartingPos();
	const size_t line = t->getLine();

	++cursor;

	/// step over '(' inserted at pass_2(...)
	++cursor;

	UserFunctionExpr::parameterList_t params = readFunctionParameters(ctxt,cursor);
	TOperator * superOp = Token::cast<TOperator>(tokens.at(cursor));

	/// fn(a).(a+1,2){} \deprecated
	ASTNode::refArray_t superConCallExpressions;
	if(superOp!=nullptr && superOp->toString()=="."){
		++cursor;
		superConCallExpressions = readExpressionsInBrackets(ctxt,cursor);
		++cursor; // step over ')'

	} /// fn(a)@(super(a+1,2)) {}
	else if(superOp!=nullptr && superOp->toString()=="@"){
		++cursor;
		if(!Token::isA<TStartBracket>(tokens.at(cursor))){
			throwError(ctxt,"Function annotation expects brackets.",superOp);
		}
		const int annotationTo = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor);

		const auto annotations = readAnnotation(ctxt,cursor+1,annotationTo-1);
		for(const auto & annotation : annotations) {
			const StringId & name = annotation.first;
			int parameterPos = annotation.second;
			log(ctxt,Logger::LOG_INFO,"Annotation:"+name.toString(),superOp );
			if(name == Consts::ANNOTATION_FN_super){
				if(parameterPos<0){
					throwError(ctxt,"'super' annotation needs parameter list.",superOp);
				}
				superConCallExpressions = readExpressionsInBrackets(ctxt,parameterPos);
			}else{
				log(ctxt,Logger::LOG_WARNING,"Annotation is invalid for functions: '"+name.toString()+"'",superOp);
			}
		}
		cursor = annotationTo+1;

	}


	ctxt.blocks.push_back(nullptr); // mark beginning of new local namespace
	Block * block = readBlockExpression(ctxt,cursor);
	block->convertToStatement();
	ctxt.blocks.pop_back(); // remove marking for local namespace

	const size_t codeEndPos = tokens.at(cursor)->getStartingPos(); // position of '}'

	/// step over ')' inserted at pass_2(...)
	++cursor;

	{	// create function expression
		UserFunctionExpr * uFunExpr = new UserFunctionExpr(block,superConCallExpressions,line);
		uFunExpr->emplaceParameterExpressions(std::move(params));	// set parameter expressions

		// store code segment in userFunction
		if(codeStartPos!=std::string::npos && codeEndPos!=std::string::npos && !ctxt.code.empty()){
			uFunExpr->setCode(CodeFragment(ctxt.code,codeStartPos,codeEndPos-codeStartPos+1));
		}
		return uFunExpr;
	}
}


/**
 * Reads a Control-Statement from tokens beginning at index "cursor".
 * Cursor is placed at the last Token of the statement.
 * @param tokens Program as Token-List.
 * @param cusror Cursor pointing at current Token.
 * @return Control-statement or an
 * \note throws an exception if no Control-Statement could be read.
 */
EPtr<AST::ASTNode> Parser::readControl(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TControl * tc = Token::cast<TControl>(tokens.at(cursor));
	if(!tc)
		throwError(ctxt,"No control found.",tokens.at(cursor));
	++cursor;

	StringId cId = tc->getId();
	/// if-Control
	if(cId==Consts::IDENTIFIER_if){
		if(!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError(ctxt,"[if] expects (",tokens.at(cursor));
		++cursor;
		EPtr<AST::ASTNode> condition = readExpression(ctxt,cursor);
		++cursor;
		if(!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"[if] expects (...)",tokens.at(cursor));
		}
		++cursor;
		const bool actionIsNormalBlock = Token::isA<TStartBlock>(tokens.at(cursor));
		EPtr<AST::ASTNode> action = readStatement(ctxt,cursor);

		// action is a loop
		if(!actionIsNormalBlock && 	action.isNotNull()){
			auto wrappingBlock = action.toType<Block>(); // no normal block, but block? -> possibly a loop wrapping block
			if(wrappingBlock && !wrappingBlock->getStatements().empty()){
				auto loop = wrappingBlock->getStatements()[0].toType<AST::LoopStatement>();
				if(loop && loop->getElseAction().isNotNull()){
					log(ctxt,Logger::LOG_WARNING, "[if] Loops using 'else' should be wrapped in a block.",tc);
				}
			}
		}
		EPtr<AST::ASTNode> elseAction;
		if((tc = Token::cast<TControl>(tokens.at(cursor+1))) && tc->getId()==Consts::IDENTIFIER_else) {
			++cursor;
			++cursor;
			elseAction = readStatement(ctxt,cursor);
		}
		return new IfStatement(condition,action,elseAction);
	}
	/// for-Control
	/*	for( [init] ; [condition] ; [incr] ) [action] else [elseAction]

		{
			[init]
		A:
			if( [condition] )
				[action]
			else
				goto else:
		continue:
			[incr]
			goto A:
		else:
			[elseAction]
		} break;
	*/
	else if(cId==Consts::IDENTIFIER_for) {
		if(!Token::isA<TStartBlock>(tokens.at(cursor))) // for{...;...;...}
			throwError(ctxt,"[for] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		loopWrappingBlock->convertToStatement();
		++cursor;
		EPtr<AST::ASTNode> initExp = readStatement(ctxt,cursor);
		if(!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		EPtr<AST::ASTNode> condition = readExpression(ctxt,cursor);
		if(!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		EPtr<AST::ASTNode> incr = readStatement(ctxt,cursor);
		if(incr.isNotNull())
			++cursor;
		if(!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects )",tokens.at(cursor));
		}
		++cursor;
		EPtr<AST::ASTNode> action = readStatement(ctxt,cursor);

		EPtr<AST::ASTNode> elseAction;
		if((tc = Token::cast<TControl>(tokens.at(cursor+1))) && tc->getId()==Consts::IDENTIFIER_else) {
			++cursor;
			++cursor;
			elseAction = readStatement(ctxt,cursor);
		}
		loopWrappingBlock->addStatement( LoopStatement::createForLoop(initExp,condition,incr,action,elseAction) );
		return loopWrappingBlock;
	}
	/// while-Control
	/*
		{
		continue:
		A:
			if( [condition] )
				[action]
			else
				[elseAction]
				break;
			goto A:
		} break:
	*/
	else if(cId==Consts::IDENTIFIER_while) {
		if(!Token::isA<TStartBlock>(tokens.at(cursor))) // while{...}
			throwError(ctxt,"[while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		loopWrappingBlock->convertToStatement();
		++cursor;
		EPtr<AST::ASTNode> condition = readExpression(ctxt,cursor);
		++cursor;
		if(!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		EPtr<AST::ASTNode> action = readStatement(ctxt,cursor);
		EPtr<AST::ASTNode> elseAction;
		if((tc = Token::cast<TControl>(tokens.at(cursor+1))) && tc->getId()==Consts::IDENTIFIER_else) {
			++cursor;
			++cursor;
			elseAction = readStatement(ctxt,cursor);
		}
		loopWrappingBlock->addStatement(  LoopStatement::createWhileLoop(condition,action,elseAction) );
		return loopWrappingBlock;
	}
	/// Do-while-Control
	/*
		{
		A:
			[action]
		continue:
			if( [condition] )
				goto A:
			else
				[elseAction]
		} break:
	*/
	else if(cId==Consts::IDENTIFIER_do) {
		EPtr<AST::ASTNode> action = readStatement(ctxt,cursor);
		++cursor;
		tc = Token::cast<TControl>(tokens.at(cursor));
		if(!tc || tc->getId()!=Consts::IDENTIFIER_while)
			throwError(ctxt,"[do-while] expects while",tokens.at(cursor));
		++cursor;
		if(!Token::isA<TStartBlock>(tokens.at(cursor))) // do{} while{...};
			throwError(ctxt,"[do-while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		loopWrappingBlock->convertToStatement();
		++cursor;
		EPtr<AST::ASTNode> condition = readExpression(ctxt,cursor);
		++cursor;
		if(!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[do-while] expects (...)",tokens.at(cursor));
		}

		EPtr<AST::ASTNode> elseAction;
		if((tc = Token::cast<TControl>(tokens.at(cursor+1))) && tc->getId()==Consts::IDENTIFIER_else) {
			++cursor;
			++cursor;
			elseAction = readStatement(ctxt,cursor);
		}else if(Token::isA<TEndCommand>(tokens.at(cursor+1))) {
			++cursor;
		}else{
			throwError(ctxt,"[do-while] expects ;",tokens.at(cursor));
		}
		loopWrappingBlock->addStatement( LoopStatement::createDoWhileLoop(condition,action,elseAction) ) ;
		return loopWrappingBlock;
	}
	/// foreach-Control
	/*	old:
		foreach( [array] as [keyIdent],[valueIndent] ) [action]

		 {  ((var __it))
				__getIterator( [array] ); == __it=[array].getIterator();
			A:
				if( __it.end() )
					break;
				__setValues(keyIdent,valueIdent); ==  keyIdent = __it.key(), valueIdent = __it.value();
				[action]
			continue:
				__it.next()
				goto A:
		 }	break:

	*/
	/*	new:
		{
			var __it;
			for( __it = obj.getIterator(); !__it.end() ; __it.next() ){
				key = __it.key();
				value = __it.key();
			}
		}
	*/

	else if(cId==Consts::IDENTIFIER_foreach) {
		if(!Token::isA<TStartBlock>(tokens.at(cursor)))  // foreach{...as...}
			throwError(ctxt,"[foreach] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		loopWrappingBlock->convertToStatement();
		++cursor;
		EPtr<AST::ASTNode> arrayExpression = readExpression(ctxt,cursor);
		++cursor;
		tc = Token::cast<TControl>(tokens.at(cursor));
		if(!tc || tc->getId()!=Consts::IDENTIFIER_as)
			throwError(ctxt,"[foreach] expects as",tokens.at(cursor));
		++cursor;

		TIdentifier * valueIdent = nullptr;
		TIdentifier * keyIdent = nullptr;
		if(!(valueIdent = Token::cast<TIdentifier>(tokens.at(cursor))))
			throwError(ctxt,"[foreach] expects Identifier-1",tokens.at(cursor));
		++cursor;

		if(Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			keyIdent = valueIdent;
			if(!(valueIdent = Token::cast<TIdentifier>(tokens.at(cursor))))
				throwError(ctxt,"[foreach] expects Identifier-2",tokens.at(cursor));
			++cursor;
		}

		if(!Token::isA<TEndBlock>(tokens.at(cursor)))
			throwError(ctxt,"[foreach] expects )",tokens.at(cursor));
		++cursor;
		EPtr<AST::ASTNode> action = readStatement(ctxt,cursor);

		static const StringId itId("__it");

		// var __it;
		loopWrappingBlock->declareLocalVar(itId);

		/* \todo speedup by using systemCall:
			for(__it = sysCall getIterator(arr); sysCall isIteratorEnd(__it); sysCall increasIterator (__it) )
		*/
		// __it = SYS_CALL_GET_ITERATOR( obj ) ( ~ __it = obj.getIterator();  + some special cases)
		ASTNode::refArray_t loopInitParams;
		loopInitParams.push_back(arrayExpression);
		EPtr<AST::ASTNode> loopInit =
			SetAttributeExpr::createAssignment(nullptr,itId,
				FunctionCallExpr::createSysCall(Consts::SYS_CALL_GET_ITERATOR,loopInitParams,tokens.at(cursor)->getLine()));

		// ! __it.end()
		EPtr<AST::ASTNode> checkExpression = LogicOpExpr::createNot(
				FunctionCallExpr::createFunctionCall(
					new GetAttributeExpr(new GetAttributeExpr(nullptr,itId), Consts::IDENTIFIER_fn_it_end),ASTNode::refArray_t() ));

		// __it.next()
		EPtr<AST::ASTNode> increaseStatement =
				FunctionCallExpr::createFunctionCall(
					new GetAttributeExpr(new GetAttributeExpr(nullptr,itId), Consts::IDENTIFIER_fn_it_next),ASTNode::refArray_t() );

		Block * actionWrapper = Block::createBlockStatement();

		// key = __it.key();
		if(keyIdent){
			actionWrapper->addStatement(
				SetAttributeExpr::createAssignment(nullptr,keyIdent->getId(),
					FunctionCallExpr::createFunctionCall(
						new GetAttributeExpr(
							new GetAttributeExpr(nullptr,itId), Consts::IDENTIFIER_fn_it_key),ASTNode::refArray_t() ),tokens.at(cursor)->getLine()));
		}

		// value = __it.value();
		if(valueIdent){
			actionWrapper->addStatement(
				SetAttributeExpr::createAssignment(nullptr,valueIdent->getId(),
					FunctionCallExpr::createFunctionCall(
						new GetAttributeExpr(
							new GetAttributeExpr(nullptr,itId), Consts::IDENTIFIER_fn_it_value),ASTNode::refArray_t() ),tokens.at(cursor)->getLine()));
		}
		actionWrapper->addStatement(action);

		loopWrappingBlock->addStatement(
				LoopStatement::createForLoop(loopInit,checkExpression,increaseStatement,
												actionWrapper));

		return loopWrappingBlock;

	}

	/// switch-case
	/*
		switch( valueExpr ){
			case caseExpr: statement* [...]
			default: statments*
		}
	*/
	else if(cId==Consts::IDENTIFIER_switch) {
		if(!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError(ctxt,"[switch] expects (",tokens.at(cursor));
		++cursor;
		EPtr<AST::ASTNode> decisionValue = readExpression(ctxt,cursor);
		++cursor;
		if(!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"[switch] expects (...)",tokens.at(cursor));
		}
		++cursor;

		TStartBlock * tsb = Token::cast<TStartBlock>(tokens.at(cursor));
		Block * block = tsb?reinterpret_cast<Block *>(tsb->getBlock()):nullptr;
		if(block==nullptr)
			throwError(ctxt,"[switch] expects {...}",tokens.at(cursor));
		++cursor;

		warnOnShadowedLocalVars(ctxt,tsb);
		ctxt.blocks.push_back(block);

		std::vector<std::pair<size_t,ERef<AST::ASTNode>>> caseDescriptions;

		bool defaultCaseRead = false;
		/// Read commands.
		while(!Token::isA<TEndBlock>(tokens.at(cursor))) {
			if(Token::isA<TEndScript>(tokens.at(cursor)))
				throwError(ctxt,"Unclosed Block {...",tsb);

			const int line = tokens.at(cursor)->getLine();
			if(Token::isA<TIdentifier>(tokens.at(cursor))){
				/// case <expression> :
				if(Token::cast<TIdentifier>(tokens.at(cursor))->getId() == Consts::IDENTIFIER_case){
					if(defaultCaseRead)
						throwError(ctxt,"[case] 'case' after 'default:'");

					++cursor;
					EPtr<AST::ASTNode> decisionExpr = readExpression(ctxt,cursor);
					++cursor;
					if(!Token::isA<TColon>(tokens.at(cursor))) {
						throwError(ctxt,"[case] ':' expected.'",tokens.at(cursor));
					}
					++cursor;
					caseDescriptions.emplace_back(block->getStatements().size(),decisionExpr);
					continue;
				}else if(Token::cast<TIdentifier>(tokens.at(cursor))->getId() == Consts::IDENTIFIER_default){
					++cursor;
					if(!Token::isA<TColon>(tokens.at(cursor)))
						throwError(ctxt,"[default] ':' expected.",tokens.at(cursor));
					++cursor;
					if(defaultCaseRead){
						throwError(ctxt,"[default] Only one default case allowed.",tokens.at(cursor));
					}
					defaultCaseRead = true;
					caseDescriptions.emplace_back(block->getStatements().size(),nullptr);
					continue;

				}
			}

			EPtr<AST::ASTNode> stmt = readStatement(ctxt,cursor);

			if(stmt.isNotNull()){
				block->addStatement(stmt);
				stmt->setLine(line);
			}
			assertTokenIsStatemetEnding(ctxt,tokens.at(cursor).get());
			++cursor;
		}
		if(!defaultCaseRead){
			caseDescriptions.emplace_back(block->getStatements().size(),nullptr);
		}

		ctxt.blocks.pop_back();
		return new SwitchCaseStatement(decisionValue,block,std::move(caseDescriptions));
	}
	/// try-catch-control
	/*
		try [tryBlock] catch(varId) {[catchBlock]}

		{
			[tryBlock]
			goto A:
		exception:
			{
			varId = currentExceptionValue
			[catchBlock]
			}
		} A:
	*/
	else if(cId==Consts::IDENTIFIER_try) {
		Block * tryBlock = readBlockExpression(ctxt,cursor);
		tryBlock->convertToStatement();

		++cursor;
		tc = Token::cast<TControl>(tokens.at(cursor));
		if(!tc || tc->getId()!=Consts::IDENTIFIER_catch)
			throwError(ctxt,"[try-catch] expects catch",tokens.at(cursor));
		++cursor;
		if(!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError(ctxt,"[try-catch] expects (",tokens.at(cursor));
		++cursor;
		TIdentifier * tIdent = nullptr;

		StringId varName;
		if((tIdent = Token::cast<TIdentifier>(tokens.at(cursor)))) {
			++cursor;
			varName = tIdent->getId();
		}

		if(!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"[try-catch] expects ([Identifier])",tokens.at(cursor));
		}
		++cursor;

		Block * catchBlock = readBlockExpression(ctxt,cursor);
		catchBlock->convertToStatement();
		return new TryCatchStatement(tryBlock,catchBlock,varName);
	}
	/// continue-Control
	else if(cId==Consts::IDENTIFIER_continue) {
		return new ContinueStatement;
	}
	/// break-Control
	else if(cId==Consts::IDENTIFIER_break) {
		return new BreakStatement;
	}
	/// return-Control
	else if(cId==Consts::IDENTIFIER_return) {
		return new ReturnStatement(readExpression(ctxt,cursor));
	}
	/// exit-Control
	else if(cId==Consts::IDENTIFIER_exit) {
		return new ExitStatement(readExpression(ctxt,cursor));
	}
	/// throw-Control
	else if(cId==Consts::IDENTIFIER_throw) {
		return new ThrowStatement(readExpression(ctxt,cursor));
	}
	/// yield-Control
	else if(cId==Consts::IDENTIFIER_yield) {
		return new YieldStatement(readExpression(ctxt,cursor));
	}
	else{
		throwError(ctxt,std::string("Parsing Unimplemented Control:")+tc->toString(),tokens.at(cursor));
		return nullptr;
	}
}

//!	getLValue
Parser::lValue_t Parser::getLValue(ParsingContext & ctxt,int from,int to,EPtr<AST::ASTNode> & obj,
								StringId & identifier,EPtr<AST::ASTNode> &indexExpression)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	/// Single Element: "a"
	if(to==from) {
		if(Token::isA<TIdentifier>(tokens[from])) {
			identifier = Token::cast<TIdentifier>(tokens.at(from))->getId();
			obj = nullptr;
			return LVALUE_MEMBER;
//        }else if(Identifier * i = dynamic_cast<Identifier *>(tokens[from])) { // $a
//            identifier = i->getId();
//            obj = nullptr;
//            return LVALUE_MEMBER;
		} else {
			throwError(ctxt,"LValue Error 1",tokens[from]);
		}
	}
	/// ".a"
	/// "a.b.c"
	if(Token::isA<TIdentifier>(tokens[to]) && Token::isA<TOperator>(tokens[to-1]) ) {
		if( Token::cast<TOperator>(tokens.at(to-1))->getOperator()->getString()==".") {
			obj = readExpression(ctxt,from,to-2);
			identifier = Token::cast<TIdentifier>(tokens[to])->getId();
			return LVALUE_MEMBER;
		}
	}
	/// ".'a'"
	/// "a.b.'c'"
	else if(TValueString * s = Token::cast<TValueString>(tokens[to])) {
		TOperator * top = Token::cast<TOperator>(tokens[to-1]);

		if(top && top->getOperator()->getString()==".") {
			obj = readExpression(ctxt,from,to-2);
			identifier = s->toString();
			return LVALUE_MEMBER;
		}
	}
	/// ".$a"
	/// "a.b.$c"
	else if(TValueIdentifier * i = Token::cast<TValueIdentifier>(tokens[to])) {
		TOperator * top = Token::cast<TOperator>(tokens[to-1]);

		if(top && top->getOperator()->getString()==".") {
			obj = readExpression(ctxt,from,to-2);
			identifier = i->getValue();
			return LVALUE_MEMBER;
		}

	}
	/// Index "a[1]"
	else if(Token::isA<TEndIndex>(tokens[to])) {

		int indexOpenPos = findCorrespondingBracket<TEndIndex,TStartIndex>(ctxt,to,from,-1);
		/// a[1]
		if(indexOpenPos>from) {
			obj = readExpression(ctxt,from,indexOpenPos-1);
			++indexOpenPos;
			indexExpression = readExpression(ctxt,indexOpenPos,to-1);
			return LVALUE_INDEX;
		}
	}
	return LVALUE_NONE;
}


/**
 * int findExpression(ctxt, cursor)
 *
 * Returns the ending Position of the next Expression, starting at cursor.
 *
 */
int Parser::findExpression(ParsingContext & ctxt,int cursor)const {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if(Token::isA<TEndScript>(tokens.at(cursor)))
		return 0;

	int level = 0;
	int to = cursor-1;
	int lastIdentifier=-10;
	int cond = 0; // number of open conditionals '?'

	Token * t = nullptr;
	while(true) {
		++to;
		t = tokens.at(to).get();

		switch(t->getType()){
			case TStartBracket::TYPE_ID:{
				TStartBracket * sb = Token::cast<TStartBracket>(t);
				if(sb->endBracketIndex>1){
					to = sb->endBracketIndex;
				}else {
					++level;
				}
				continue;
			}
			case TStartBlock::TYPE_ID:
			case TStartMap::TYPE_ID:
			case TStartIndex::TYPE_ID:{
				++level;
				continue;
			}
			case TEndBlock::TYPE_ID:
			case TEndBracket::TYPE_ID:
			case TEndMap::TYPE_ID:
			case TEndIndex::TYPE_ID:{
				level--;

				if(level<0) {
					to--;
					return to;
				}
				continue;
			}
			case TEndScript::TYPE_ID:{
				if(level==1)
					return to;

				throwError(ctxt,"Unexpected Ending.",tokens.at(cursor));
			}
//
			default:{
			}
		}
		if(level>0)
			continue;
		switch(t->getType()){
			case TControl::TYPE_ID: {
				if(Token::cast<TControl>(t)->getId()==Consts::IDENTIFIER_as) {
					to--;
					return to;
				}
				throwError(ctxt,"Expressions cannot contain control statements.",t);
			}
			case TEndCommand::TYPE_ID:{
				return to;
			}
			case TDelimiter::TYPE_ID:
			case TMapDelimiter::TYPE_ID:
			case TColon::TYPE_ID:{
				if(cond==0){
					--to;
					return to;
				}else{
					--cond;
					continue;
				}
			}
			case TIdentifier::TYPE_ID:{
				if(lastIdentifier==to-1){
					to--;
					return to;
				}
				lastIdentifier = to;
				continue;
			}
			case TOperator::TYPE_ID:{
				if(Token::cast<TOperator>(t)->toString()=="?")
					++cond;
				continue;
			}
			default:{
			}
		}
	}
	return to;
}
/**
 * e.g. (a, Number b, c = 2+3)
 * Cursor is moved after the Parameter-List.
 */
UserFunctionExpr::parameterList_t Parser::readFunctionParameters(ParsingContext & ctxt,int & cursor)const  {
	UserFunctionExpr::parameterList_t params;
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if(!Token::isA<TStartBracket>(tokens.at(cursor))) {
		return params;
	}
	++cursor;
	// fn (bla,blub,)
	bool first = true;
	uint8_t multiParamState = 0; // 0...no multi param found, 1...current param is multi param, 2...multiParam already set
	while(true) { // foreach parameter
		if(first&&Token::isA<TEndBracket>(tokens.at(cursor))) {
			++cursor;
			break;
		}
		first = false;

//		{ // ignore additional parameters: ...)
//			Token * t = tokens.at(cursor).get();
//			if(Token::isA<TOperator>(t) && (t->toString()=="...") && Token::isA<TEndBracket>(tokens.at(cursor+1).get()) ){
//				params.emplace_back(StringId()); // add empty parameter
//				params.back().setMultiParam(true);
//				if(multiParamState!=0)
//					throwError(ctxt,"[fn] Only one multi parameter (...) allowed.",tokens.at(cursor));
//				cursor+=2;
//				break;
//			}
//		}

		/// Parameter::= Expression? Identifier ( ('=' Expression)? ',') | ('*'|'...'? ('=' Expression)? ')')
		int c = cursor;

		// find identifierName, its position, the default expression and identify a multiParam
		int idPos=-1;
		StringId name;
		EPtr<AST::ASTNode> defaultExpression = nullptr;

		while(true){
			Token * t = tokens.at(c).get();
			if(Token::isA<TIdentifier>(t)) {
				// this may not be the final identifier...
				name = Token::cast<TIdentifier>(t)->getId();
				idPos = c;

				Token * tNext = tokens.at(c+1).get();
				// '*'|'...' ?
				if(  Token::isA<TOperator>(tNext) && (tNext->toString()=="..." || tNext->toString()=="*" )){
					if(multiParamState!=0)
						throwError(ctxt,"[fn] Only one multi parameter (...) allowed.",tokens.at(cursor));
					multiParamState = 1;
					++c;
					tNext = tokens.at(c+1).get();
				}
				// ',' | ')'
				if( Token::isA<TEndBracket>(tNext)){
					break;
				}else if( Token::isA<TDelimiter>(tNext) ) {
					break;
				}else if(  Token::isA<TOperator>(tNext) && tNext->toString()=="=" ){
					int defaultExpStart = c+2;
					int defaultExpTo = findExpression(ctxt,defaultExpStart);
					defaultExpression = readExpression(ctxt,defaultExpStart,defaultExpTo);
					if(defaultExpression==nullptr) {
						throwError(ctxt,"[fn] SyntaxError in default parameter.",tokens.at(cursor));
					}
					c = defaultExpTo;
					break;
				}
			}else if(Token::isA<TStartBracket>(t)){
				c = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,c);
			}else if(Token::isA<TStartIndex>(t)){
				c = findCorrespondingBracket<TStartIndex,TEndIndex>(ctxt,c);
			}else if(Token::isA<TStartMap>(t)){
				c = findCorrespondingBracket<TStartMap,TEndMap>(ctxt,c);
			}else if(Token::isA<TOperator>(t) && t->toString()=="..." &&
						(Token::isA<TEndBracket>(tokens.at(c+1))||Token::isA<TDelimiter>(tokens.at(c+1)))){
				// empty multi-parameter fn(a,...,b)
				if(multiParamState!=0)
					throwError(ctxt,"[fn] Only one multi parameter (...) allowed.",tokens.at(cursor));
				multiParamState = 1;
				break;
			}else if(Token::isA<TEndScript>(t) || Token::isA<TEndBracket>(t)){
				throwError(ctxt,"[fn] Error in parameter definition.",t);
			}
			++c;
		}

		// get the type-check-expressions
		ASTNode::refArray_t typeExpressions;
		if(	idPos>cursor ){
			int c2 = cursor;
			Token * t = tokens.at(c2).get();

			// multiple possibilities: fn([Number,'yes'] a){...}
			if(Token::isA<TStartIndex>(t)){
				++c2;
				do {
					if(Token::isA<TDelimiter>(tokens.at(c2))){ // empty expression (1,,2)
						throwError(ctxt,"Expected ]",tokens.at(c2));
					}
					typeExpressions.push_back(readExpression(ctxt,c2));
					++c2;
					if(Token::isA<TDelimiter>(tokens.at(c2))){
						++c2;
					}else if(!Token::isA<TEndIndex>(tokens.at(c2))) {
						throwError(ctxt,"Expected ]",tokens.at(c2));
					}
				}while(!Token::isA<TEndIndex>(tokens.at(c2)));

			} // single type criterium: fn( Bool a){...}
			else{
				typeExpressions.push_back(readExpression(ctxt,c2,idPos-1));
			}
		}

		// test if this is the last parameter
		bool lastParam = false;
		if(Token::isA<TEndBracket>(tokens[c+1])){
			lastParam = true;
		}else if( ! Token::isA<TDelimiter>(tokens[c+1])){
			throwError(ctxt,"[fn] SyntaxError.",tokens[c+1]);
		}

		// move cursor
		cursor = c+2;

		// create parameter
		params.emplace_back(name,nullptr,std::move(typeExpressions));
		if(multiParamState==1){
			params.back().setMultiParam(true);
			multiParamState = 2;
		}
		if(defaultExpression!=nullptr)
			params.back().setDefaultValueExpression(defaultExpression);

		if(lastParam){
			break;
		}
	}
	return params;
}

/*!	1,bla+2,(3*3)
	Cursor is moved at closing bracket ')'
*/
ASTNode::refArray_t Parser::readExpressionsInBrackets(ParsingContext & ctxt,int & cursor)const{
	ASTNode::refArray_t expressions;
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t = tokens.at(cursor).get();
	if(t->toString()!="(") {
		throwError(ctxt,"Expression list error.",t);
	}
	++cursor;

	while(!Token::isA<TEndBracket>(tokens.at(cursor))) {
		if(Token::isA<TDelimiter>(tokens.at(cursor))){ // empty expression (1,,2)
			expressions.push_back(nullptr);
			++cursor;
			continue;
		}
		expressions.emplace_back(readExpression(ctxt,cursor));
		++cursor;
		if(Token::isA<TDelimiter>(tokens.at(cursor))){
			++cursor;
		}else if(!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"Expected )",tokens.at(cursor));
		}
	}
	return expressions;
}

/*! search for expanding parameters f(0,arr...,2,3)
	If found, the "..." is removed and its index is stored.	*/
std::vector<uint32_t> Parser::extractExpandingParameters(std::vector<ERef<AST::ASTNode>> & paramExps)const{
	static const StringId multiParamOp("..._post");

	std::vector<uint32_t> expandingParameters;
	int i=0;
	for(auto & pExp : paramExps){
		++i;
		if(pExp.isNull() || pExp->getNodeType() != ASTNode::TYPE_FUNCTION_CALL_EXPRESSION)
			continue;
		ASTNode::ptr_t gfe = static_cast<FunctionCallExpr*>(pExp.get())->getGetFunctionExpression();
		if(gfe.isNull() || gfe->getNodeType() != ASTNode::TYPE_GET_ATTRIBUTE_EXPRESSION)
			continue;
		GetAttributeExpr * gae = static_cast<GetAttributeExpr*>(gfe.get());
		if( gae->getAttrId()!=multiParamOp )
			continue;
		pExp = gae->getObjectExpression();
		expandingParameters.push_back(i-1);
	}
	return expandingParameters;
}

/**
 * A.m @(const,private,somthingWithOptions("foo")) := ...
 *       ^from                            ^p    ^to
 * ---> [ ($const,-1),($private,-1),($somthingWithOptions,p) ]
 */
Parser::annotations_t Parser::readAnnotation(ParsingContext & ctxt,int from,int to)const{
	annotations_t annotations;
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	for(int cursor = from;cursor<=to;++cursor){
		Token * t = tokens.at(cursor).get();
		const TIdentifier * tid = Token::cast<TIdentifier>(t);
		if( tid==nullptr )
			throwError(ctxt,"Identifier expected in annotation",t);

		int optionPos = -1;
		++cursor;
		if( Token::isA<TStartBracket>(tokens.at(cursor)) && cursor < to ){ // annotation has options 'annotation(exp1,exp2)'
			optionPos = cursor;
			cursor = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to); // skip expressions in brackets
			if(cursor<0)
				throwError(ctxt,"Unclosed option list in annotations",tokens.at(cursor));
			++cursor; // skip ')'
		}
		if(cursor<=to && !Token::isA<TDelimiter>(tokens.at(cursor))){ // expect a delimiter or the end.
			throwError(ctxt,"Syntax error in annotation.",tokens.at(cursor));
		}
		annotations.emplace_back( tid->getId(),optionPos);
	}
	return annotations;
}


void Parser::throwError(ParsingContext & ctxt,const std::string & msg,Token * token)const{
	ParserException * e = new ParserException(msg,token);
	e->setFilename(ctxt.code.getFilename());
	throw e;
}

}
