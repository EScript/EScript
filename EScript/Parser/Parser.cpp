// Parser.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Parser.h"
#include "Tokenizer.h"
#include "Operators.h"
#include "../EScript.h"
#include "../Objects/AST/FunctionCallExpr.h"
#include "../Objects/AST/GetAttributeExpr.h"
#include "../Objects/AST/SetAttributeExpr.h"
#include "../Objects/AST/IfStatement.h"
#include "../Objects/AST/ConditionalExpr.h"
#include "../Objects/AST/ForeachStatement.h"
#include "../Objects/AST/LogicOpExpr.h"
#include "../Objects/AST/LoopStatement.h"
#include "../Objects/AST/Statement.h"
#include "../Objects/AST/TryCatchStatement.h"

#include "../Objects/Values/Number.h"
#include "../Objects/Values/String.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Objects/YieldIterator.h"
#include "../Objects/Identifier.h"

#include "../Utils/IO/IO.h"

#include <stdio.h>
#include <stack>
#include <sstream>

namespace EScript {
using namespace AST;
using std::string;
// -------------------------------------------------------------------------------------------------------------------
// helper

template<class BracketStart,class BracketEnd>
int findCorrespondingBracket(const Parser::ParsingContext & ctxt,int from,int to=-1,int direction=1){
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<BracketStart>(tokens.at(from))){
		std::cerr << "Unkwown error in brackets (should not happen!)\n";
		return -1;
	}
	int cursor=from;
	int level=1;
	while(cursor != to){
		cursor+=direction;
		if (Token::isA<BracketStart>(tokens.at(cursor))){
			level++;
		}else if (Token::isA<BracketEnd>(tokens.at(cursor))){
			level--;
		}else if (Token::isA<TEndScript>(tokens.at(cursor))){
			return -1;
		}
		if (level==0)
			return cursor;
	}
	return -1;
}

// -------------------------------------------------------------------------------------------------------------------
//! (static)
Type * Parser::getTypeObject(){
	// [Parser] ---|> [Object]
	static Type * typeObject=new Type(Object::getTypeObject());
	return typeObject;
}


//! (static) initMembers
void Parser::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] Parser new Parser();  @deprecated
	ESF_DECLARE(typeObject,"_constructor",0,0,new Parser(runtime.getLogger()))

	//!	[ESMF] BlockStatement Parser.parse(String) @deprecated
	ES_MFUNCTION_DECLARE(typeObject,Parser,"parse",1,1,{
		ERef<BlockStatement> block(new BlockStatement());
		static const StringId inline_id("[inline]");
		block->setFilename(inline_id);
		try {
			self->parse(block.get(),StringData(parameter[0]->toString()));
		} catch (Exception * e) {
			runtime.setException(e); // adds stack info
			return NULL;
		}
		return block.detachAndDecrease();
	})

	//!	[ESMF] BlockStatement Parser.parseFile(String filename) @deprecated
	ESMF_DECLARE(typeObject,Parser,"parseFile",1,1, self->parseFile(parameter[0]->toString()))
}

// -------------------------------------------------------------------------------------------------------------------

//!	(ctor)
Parser::Parser(Logger * _logger,Type * type) :
		Object(type?type:getTypeObject()), _produceBytecode(false),
		logger(_logger ? _logger : new StdLogger(std::cout)) {
	//ctor
}

//!	(dtor)
Parser::~Parser() {
	//dtor
}

//!	---|> [Object]
Object * Parser::clone()const {
	return new Parser();
}

void Parser::log(Logger::level_t messageLevel, const std::string & msg,const _CountedRef<Token> & token)const{
	std::ostringstream os;
	os << "[Parser] " << msg << " (" << getCurrentFilename();
	if(token!=NULL)
		os << ':' << token->getLine();
	os << ").";
	logger->log(messageLevel,os.str());
}

//! Loads and parses a File.
BlockStatement * Parser::parseFile(const std::string & filename) {
	StringData content;
	try{
		content = IO::loadFile(filename);
	}catch(const std::ios::failure & e){
		throwError(e.what());
	}

	ERef<BlockStatement> rootBlock(new BlockStatement);
	rootBlock->setFilename(filename);
	parse(rootBlock.get(),content);
	return rootBlock.detachAndDecrease();
}

//! Parse a CString.
Object * Parser::parse(BlockStatement * rootBlock,const StringData & c) {
	tokenizer.defineToken("__FILE__",new TObject(String::create(rootBlock->getFilename())));
	tokenizer.defineToken("__DIR__",new TObject(String::create(IO::dirname(rootBlock->getFilename()))));

	Tokenizer::tokenList_t tokens;
	ParsingContext ctxt(tokens,String::create(c));
	ctxt.rootBlock=rootBlock;
	currentFilename = rootBlock->getFilename();

	/// 1. Tokenize
	try {
		tokenizer.getTokens(c.str().c_str(),tokens);
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

	/// 3. Parse expressions and finally add them up the script.
	int cursor=0;
	Statement statement=getStatement(ctxt,cursor);

	return statement.getExpression();
}

//! [Helper]
struct _BlockInfo {
	Token * token;
	unsigned int index;
	bool isCommandBlock;
	bool empty;
	bool containsColon;
	bool containsCommands;
	int shortIf; // a?b:c

	_BlockInfo(unsigned int _index=0,Token * _token=NULL):
			token(_token),index(_index),
			isCommandBlock(Token::isA<TStartBlock>(token)),empty(true),
			containsColon(false),containsCommands(false),shortIf(0) {};
	_BlockInfo(const _BlockInfo & b):
			token(b.token),index(b.index),
			isCommandBlock(b.isCommandBlock),empty(b.empty),
			containsColon(b.containsColon),containsCommands(b.containsCommands),shortIf(b.shortIf) {
//            std::cout << " ####### \n";
	};

};
/**
 * Pass 1
 * =========
 * - check Syntax of Brackets () [] {}
 * - disambiguate Map/BlockStatement
 * - colon ( Mapdelimiter / shortIf ?:)
 */
void Parser::pass_1(ParsingContext & ctxt) {
	Tokenizer::tokenList_t & tokens = ctxt.tokens;

	std::stack<_BlockInfo> bInfStack;
	bInfStack.push(_BlockInfo());

	for(size_t cursor=0;cursor<tokens.size();++cursor) {
		Token * token=tokens.at(cursor).get();
		/// currentBlockInfo
		_BlockInfo & cbi=bInfStack.top();

		switch(token->getType()){
			case TStartBracket::TYPE_ID:
			case TStartBlock::TYPE_ID:
			case TStartIndex::TYPE_ID:{
				bInfStack.push(_BlockInfo(cursor,token));
				continue;
			}
			case TEndBracket::TYPE_ID:{
				if (!Token::isA<TStartBracket>(cbi.token)) {
					throwError("Syntax Error: ')'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndIndex::TYPE_ID:{
				if (!Token::isA<TStartIndex>(cbi.token)) {
					throwError("Syntax Error: ']'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndBlock::TYPE_ID:{
				if (!cbi.isCommandBlock) {
					throwError("Syntax Error: '}'",token);
				}
				/// BlockStatement is Map Constructor
				if ( cbi.containsColon) {
					unsigned int startIndex=cbi.index;

					Token * t=new TStartMap();
					if( !tokens.at(startIndex).isNull() )
						t->setLine((tokens.at(startIndex))->getLine());
//					Token::removeReference(tokens.at(startIndex));

					tokens[startIndex]=t;
//					Token::addReference(t);

					t=new TEndMap();
					if( !tokens.at(cursor).isNull() )
						t->setLine((tokens.at(cursor))->getLine());
//					Token::removeReference(tokens.at(cursor));

					tokens[cursor]=t;
//					Token::addReference(t);
				}
				bInfStack.pop();
				continue;
			}
			default:{
			}
		}

		if (cbi.isCommandBlock) {
			cbi.empty=false;
			if (Token::isA<TColon>(token) ){
				if (cbi.shortIf>0) {
					cbi.shortIf--;
				} else if (cbi.containsCommands) {
					throwError("Syntax Error in BlockStatement: ':'",token);
				} else {
					cbi.containsColon=true;
					Token * t=new TMapDelimiter();

					if((tokens.at(cursor))!=NULL)
						t->setLine((tokens.at(cursor).get())->getLine());
//					Token::removeReference(tokens.at(cursor));

					tokens[cursor]=t;
//					Token::addReference(t);
					continue;
				}
			} else if (Token::isA<TEndCommand>(token)) {
				if (cbi.containsColon) {
					throwError("Syntax Error in Map: ';'",token);
				}
				cbi.containsCommands=true;
				cbi.shortIf=0;
			} else if (Token::isA<TOperator>(token) && token->toString()=="?") {
				cbi.shortIf++;
			}
		}
	}
	//std::cout << "\n###"<<tStack.top()->toString();
	if (bInfStack.top().token!=NULL) {
		throwError("Unexpected eof (unclosed '"+bInfStack.top().token->toString()+"'?)",bInfStack.top().token);
	}
}

/**
 * Pass 2
 * =========
 * - Create BlockStatement-Objects
 * - Parse declarations (var)
 * - Wrap parts of "fn" in brackets for easier processing: fn(foo,bar){...}  --->  fn( (foo,bar){} )
 * - Change loop brackets to blocks to handle loop wide scope handling: while(...) ---> while{...}
 * - TODO: Class declaration
 * ?????- TODO: Undefined scope for i: "{ var i;  do{ var i; }while(var i); }"
 */
void Parser::pass_2(ParsingContext & ctxt,
					Tokenizer::tokenList_t & enrichedTokens)const  {

	std::stack<BlockStatement *> blockStack;
	blockStack.push(ctxt.rootBlock);

	/// Counts the currently open brackets and blocks for the current function declaration.
	/// If the top value reaches 0 after reading a TEndBlock, the fn-wrapper brackets have to be closed.
	std::stack<int> functionBracketDepth;

	std::stack<TStartBracket*> currentBracket;

	/// for(...) ---> for{...}
	std::stack<Token*> loopConditionEndingBrackets;

	enrichedTokens.reserve(ctxt.tokens.size());

	TStartBlock * tsb=new TStartBlock(ctxt.rootBlock);
//	Token::addReference(tsb);
	enrichedTokens.push_back(tsb);

	for (size_t cursor=0;cursor<ctxt.tokens.size();++cursor) {
		Token * token=ctxt.tokens.at(cursor).get();

		/// for(...) ---> for{...}
		if(!loopConditionEndingBrackets.empty() && token == loopConditionEndingBrackets.top()){
			loopConditionEndingBrackets.pop();
			Token * t=new TEndBlock();
			t->setLine(token->getLine());
//			Token::addReference(t);
			token=t;
		}


		switch(token->getType()){
			case TControl::TYPE_ID:{
				TControl * tc=Token::cast<TControl>(token);
				/// Variable Declaration
				if (tc->getId()==Consts::IDENTIFIER_var) {
					if (TIdentifier * ti=Token::cast<TIdentifier>(ctxt.tokens.at(cursor+1))) {
						if(!blockStack.top()->declareVar(ti->getId())){
							log(Logger::LOG_WARNING, "Duplicate local variable '"+ti->toString()+'\'',ti);
						}
//						Token::removeReference(token);
						continue;
					} else
						throwError("var expects Identifier.",tc);
				}
				/// for(...) ---> for{...}
				else if(tc->getId()==Consts::IDENTIFIER_for || tc->getId()==Consts::IDENTIFIER_foreach || tc->getId()==Consts::IDENTIFIER_while){
					if( ctxt.tokens.at(cursor+1)->getType()!=TStartBracket::TYPE_ID )
						throwError(tc->toString()+" expects '('",tc);
					int endPos = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor+1);
					if(endPos<0)
						throwError("Error in loop condition",tc);
					loopConditionEndingBrackets.push(ctxt.tokens.at(endPos).get());

					enrichedTokens.push_back(token);
					++cursor;
					BlockStatement * loopConditionBlock=new BlockStatement(tc->getLine());
					loopConditionBlock->setFilename(currentFilename); /// debugging information:
					blockStack.push(loopConditionBlock);

					TStartBlock * sb=new TStartBlock(loopConditionBlock);
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
			/// Open new BlockStatement
			case TStartBlock::TYPE_ID:{
				TStartBlock * sb=Token::cast<TStartBlock>(token);
				BlockStatement * currentBlock=new BlockStatement(sb->getLine());//currentBlock);

				/// debugging information:
				currentBlock->setFilename(currentFilename);

				blockStack.push(currentBlock);
				sb->setBlock(currentBlock);
				enrichedTokens.push_back(token);

				if(!functionBracketDepth.empty())
					++functionBracketDepth.top();

				continue;
			}
			/// Close BlockStatement
			case TEndBlock::TYPE_ID:{
				enrichedTokens.push_back(token);

				blockStack.pop();
				if (blockStack.empty())
					throwError("Unexpected }");

				if(!functionBracketDepth.empty()){
					--functionBracketDepth.top();

					if(functionBracketDepth.top()==0){
						functionBracketDepth.pop();
						Token * t=new TEndBracket();
						t->setLine(token->getLine());

//						Token::addReference(t);
						enrichedTokens.push_back(t);

						// add shortcut to the closing bracket
						currentBracket.top()->endBracketIndex=enrichedTokens.size()-1;
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
					throwError("Missing opening bracket for ",token);

				// add shortcut to the closing bracket
				currentBracket.top()->endBracketIndex=enrichedTokens.size()-1;
				currentBracket.pop();
				continue;
			}

			/// fn(foo,bar){...}  ---> fn( (foo,bar){} )
			case TOperator::TYPE_ID:{
				enrichedTokens.push_back(token);
				if( token->toString() == "fn"  ) {//|| token->toString() == "lambda") {
					functionBracketDepth.push(0);
					TStartBracket * t=new TStartBracket();
					t->setLine(token->getLine());
					currentBracket.push(t);
//					Token::addReference(t);
					enrichedTokens.push_back(t);
				}
				continue;
			}
			/// End of script
			case TEndScript::TYPE_ID:{
				blockStack.pop();
				if (!blockStack.empty())
					throwError("Unclosed {");

				Token * t=new TEndBlock();
				t->setLine(token->getLine());
//				Token::addReference(t);
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

/**
 *
 * Object * getExpression(...)
 *
 *
 * Cursor is moved to the last position of the Expression.
 *
 */
Object * Parser::getExpression(ParsingContext & ctxt,int & cursor,int to)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (cursor>=static_cast<int>(tokens.size())){
		return NULL;
	}/// Commands: if(...){}
	else if (Token::isA<TControl>(tokens.at(cursor))) {
		log(Logger::LOG_WARNING, "No control statement here!",tokens.at(cursor));
		return NULL;
	} /// BlockStatement: {...}
	else if (Token::isA<TStartBlock>(tokens.at(cursor))) {
		return getBlock(ctxt,cursor);
	}

	/// If "to" is not given, search the end of the expression
	if (to==-1) {
		to=findExpression(ctxt,cursor);
	}

	/// Only happens when searching for non existing Expression:
	///  the empty side of binary Expression (empty)!a or a++(empty)
	if (to<cursor) {
		return NULL;
	}

	///  Single Element
	/// -------------------
	else if (to==cursor) {
		Token *t =tokens.at(cursor).get();

		/// Empty Command
		if (Token::isA<TEndCommand>(t)) {
			return NULL;
		}else if(TObject * tObj=Token::cast<TObject>(t)){
//        	std::cout << "found obj: "<<tObj->obj.get()->toString()<<"\n";
			return tObj->obj.get();
		}
		///  Identifier
		/// "a" => "_.get('a')"
		else if (TIdentifier * ident=Token::cast<TIdentifier>(t)) {
		// is local variable?
			for(int i=ctxt.blocks.size()-1;i>=0;--i){
				BlockStatement * b=ctxt.blocks.at(i);
				if(b==NULL)
					break;
				 else if(b->isLocalVar(ident->getId())){
//////					std::cout <<"local:"<<ident->toString()<<"\n";
					break;
				 }
		}
			return new GetAttributeExpr(NULL,ident->getId());  // ID
		}
		throwError("Unknown (or unimplemented) Token",t);
	}

	///  Command ends with ;
	///  "2;"
	/// ---------------------
	else if (Token::isA<TEndCommand>(tokens[to])) {
		Object * e=getExpression(ctxt,cursor,to-1);
		cursor=to;
		return e;
	}

	/// Surrounded with Brackets
	/// "(a+2)"
	/// --------------------------
	else if (Token::isA<TStartBracket>(tokens.at(cursor)) &&
			 Token::isA<TEndBracket>(tokens[to]) &&
			 findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to,1)==to) {

		++cursor;
		Object * innerExpression=getExpression(ctxt,cursor,to-1);
		cursor=to;
		return innerExpression;
	}

	/// Map Constructor
	/// "{foo:bar,2:3}"
	/// --------------------------
	if (Token::isA<TStartMap>(tokens.at(cursor)) &&
		Token::isA<TEndMap>(tokens[to]) &&
		findCorrespondingBracket<TStartMap,TEndMap>(ctxt,cursor,to,1) == to) {
		return getMap(ctxt,cursor);
	}

	/// BinaryExpression
	/// "3+foo"
	/// --------------------------
	if (Object * obj=getBinaryExpression(ctxt,cursor,to)) {
		return obj;
	}

	///    Syntax Error
	/// --------------------
	else {
		throwError("Syntax error",tokens.at(cursor).get());
		return NULL;
	}
}

//! (internal)
Statement Parser::createStatement(Object *exp)const{
	if(exp==NULL)
		return Statement(Statement::TYPE_UNDEFINED);
	if( dynamic_cast<BlockStatement*>(exp) ){
		return Statement(Statement::TYPE_BLOCK,exp);
	}else if( dynamic_cast<IfStatement*>(exp) ){
		return Statement(Statement::TYPE_IF,exp);
	}
	return Statement(Statement::TYPE_EXPRESSION,exp);
}

//! (internal)
Statement Parser::getStatement(ParsingContext & ctxt,int & cursor,int to)const{

	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (Token::isA<TControl>(tokens.at(cursor))) {
		return getControl(ctxt,cursor);
	} /// sub-BlockStatement: {...}
	else if (Token::isA<TStartBlock>(tokens.at(cursor))) {
		return Statement(Statement::TYPE_BLOCK, getBlock(ctxt,cursor));
	}else{
		Object * exp=getExpression(ctxt,cursor,to);
		if(exp)
			return Statement(Statement::TYPE_EXPRESSION,exp);
		return Statement(Statement::TYPE_UNDEFINED);
	}

}

/**
 * Get block of statements
 * {out("foo");exit;}
 */
BlockStatement * Parser::getBlock(ParsingContext & ctxt,int & cursor)const {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TStartBlock * tsb=Token::cast<TStartBlock>(tokens.at(cursor));
	BlockStatement * b=tsb?reinterpret_cast<BlockStatement *>(tsb->getBlock()):NULL;
	if (b==NULL)
		throwError("No BlockStatement!",tokens.at(cursor));

	/// Check for shadowed local variables
	if(!ctxt.blocks.empty()){
		const BlockStatement::declaredVariableMap_t * vars = b->getVars();
		if(vars!=NULL){
			for(int i = ctxt.blocks.size()-1; i>=0 && ctxt.blocks[i]!=NULL; --i ){
				const BlockStatement::declaredVariableMap_t * vars2 = ctxt.blocks[i]->getVars();
				if(vars2==NULL)
					continue;
				for(BlockStatement::declaredVariableMap_t::const_iterator it=vars->begin();it!=vars->end();++it){
					if(vars2->count(*it)>0){
						log(Logger::LOG_PEDANTIC_WARNING,"Shadowed local variable  '"+(*it).toString()+"' in block.",tokens.at(cursor));
					}

				}
			}
		}
	}

	ctxt.blocks.push_back(b);

	++cursor;


	/// Read commands.
	while (!Token::isA<TEndBlock>(tokens.at(cursor))) {
		if (Token::isA<TEndScript>(tokens.at(cursor)))
			throwError("Unclosed BlockStatement {...",tsb);

		int line=tokens.at(cursor)->getLine();
		Statement stmt=getStatement(ctxt,cursor);

		if(stmt.isValid()){
			stmt.setLine(line);
			b->addStatement(stmt);
		}

		/// Commands have to end on ";" or "}".
		if (!(Token::isA<TEndCommand>(tokens.at(cursor)) || Token::isA<TEndBlock>(tokens.at(cursor)))) {
			log(Logger::LOG_DEBUG, tokens.at(cursor)->toString(),tokens.at(cursor));
			throwError("Syntax Error in BlockStatement.",tokens.at(cursor));
		}
		++cursor;
	}
	ctxt.blocks.pop_back();
	return b;
}

//!	getMap
Object * Parser::getMap(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<TStartMap>(tokens.at(cursor)))
		throwError("No Map!",tokens.at(cursor));

	// for debugging
	int currentLine=-1;
	{
		Token * t=tokens.at(cursor).get();
		if (t)
			currentLine=t->getLine();
	}

	++cursor;

	Object * exp=NULL;
	std::vector<ObjRef> paramExp;
	while (!Token::isA<TEndMap>(tokens.at(cursor))) {

		/// i) read Key

		/// Key is not present
		if (Token::isA<TMapDelimiter>(tokens.at(cursor))) {
			exp=Void::get();
		} /// Key is present
		else {
			exp=getExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		/// ii) read ":"
		if (!Token::isA<TMapDelimiter>(tokens.at(cursor))) {
			log(Logger::LOG_DEBUG, tokens.at(cursor)->toString(),tokens.at(cursor));
			throwError("Map: Expected : ",tokens.at(cursor));
		}
		++cursor;

		/// iii) read Value
		/// Value is not present (only valid for last tuple)
		if (Token::isA<TEndMap>(tokens.at(cursor))) {
			exp=Void::get();
		} /// Value is present
		else {
			exp=getExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		if (Token::isA<TEndMap>(tokens.at(cursor)))
			break;
		else if (Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			continue;
		} else
			throwError("Map Syntax Error",tokens.at(cursor));
	}

	FunctionCallExpr * funcCall = new FunctionCallExpr(
					Map::getTypeObject()->getAttribute(Consts::IDENTIFIER_fn_constructor).getValue(),
					paramExp,false,currentFilename,currentLine);
	return funcCall;
}

/*!	Binary expression	*/
Object * Parser::getBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	int currentLine = tokens.at(cursor).isNull() ? -1 : tokens.at(cursor)->getLine();

	int opPosition=-1; /// Position of operator with lowest precedence
	int opPrecedence=-1; /// Highest precedence
	const Operator * op=NULL;

	/// search operator with lowest precedence

	int level=0; /// BracketLevel ( ) [] {}
	for (int i=cursor;i<=to;++i) {
		Token * t=tokens.at(i).get();
		if (level==0) {
			TOperator * top=Token::cast<TOperator>(t);
			if (top &&
					(top->getAssociativity()==Operator::L?
					 top->getPrecedence() >= opPrecedence :
					 top->getPrecedence() > opPrecedence)) {
				opPrecedence=top->getPrecedence();
				opPosition=i;
				op=top->getOperator();
				currentLine=top->getLine();
			}
		}
		if (Token::isA<TStartBlock>(t)
				||Token::isA<TStartBracket>(t)
				||Token::isA<TStartIndex>(t)
				||Token::isA<TStartMap>(t)) {
			level++;
			continue;
		} else if (Token::isA<TEndBlock>(t)
				   ||Token::isA<TEndBracket>(t)
				   ||Token::isA<TEndIndex>(t)
				   ||Token::isA<TEndMap>(t)) {
			level--;
			if (level<0) {
				throwError("Error in binary expression",t);
			}
			continue;

		}
	}
	if (opPosition<0 || !op) return NULL;

	int rightExprFrom=opPosition+1;
	int leftExprFrom=cursor,leftExprTo=opPosition-1;

	/// ASSIGNMENTS ( "="  ":=" )
	/// -----------
	if (op->getString()=="=") {
		StringId memberIdentifier;
		Object * obj=NULL;
		Object * indexExp=NULL;
		int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=getExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;


		/// a=2 => _.[a] = 2
		if (lValueType== LVALUE_MEMBER) {
			return SetAttributeExpr::createAssignment(obj,memberIdentifier,rightExpression,currentLine);
		}
		/// a[1]=2 =>  _.a._set(1, 2)
		else if (lValueType == LVALUE_INDEX) {
			std::vector<ObjRef> paramExp;
			paramExp.push_back(indexExp);
			paramExp.push_back(rightExpression);
			return new FunctionCallExpr(new GetAttributeExpr(obj,Consts::IDENTIFIER_fn_set),paramExp,false,currentFilename,currentLine);
		} else {
//			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throwError("No valid LValue before '=' ",tokens[opPosition]);
		}
	} else if (op->getString()==":=" || op->getString()=="::=") {
		Attribute::flag_t flags = op->getString()=="::=" ? Attribute::TYPE_ATTR_BIT : 0;
		Attribute::flag_t inverseFlags = 0;

		/// extract properties:  Object.member @(property*) := value
		///                       leftExpr      property        rightExpr
		if( Token::isA<TEndBracket>(tokens[leftExprTo]) ){
			int propertyStart = findCorrespondingBracket<TEndBracket,TStartBracket>(ctxt,leftExprTo,leftExprFrom,-1);
			TOperator * atOp = Token::cast<TOperator>(tokens.at(propertyStart-1));
			if(propertyStart>0 && atOp!=NULL && atOp->toString()=="@"){
				properties_t properties;
				readProperties(ctxt,propertyStart+1,leftExprTo-1,properties);
				leftExprTo = propertyStart-2;

				for(properties_t::const_iterator it=properties.begin();it!=properties.end();++it ){
					const StringId name = it->first;
					log(Logger::LOG_INFO,"Property:"+name.toString(),atOp );
//					const int pos = it->second;
					if(name == Consts::PROPERTY_ATTR_const){
						flags |= Attribute::CONST_BIT;
					}else if(name == Consts::PROPERTY_ATTR_init){
						if(flags&Attribute::TYPE_ATTR_BIT)
							log(Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
						flags |= Attribute::INIT_BIT;
					}else if(name == Consts::PROPERTY_ATTR_member){
						if(flags&Attribute::TYPE_ATTR_BIT){
							log(Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::TYPE_ATTR_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_override){
						flags |= Attribute::OVERRIDE_BIT;
					}else if(name == Consts::PROPERTY_ATTR_private){
						if(inverseFlags&Attribute::PRIVATE_BIT){
							log(Logger::LOG_WARNING,"'@(private)' is used in combination with @(public) and is ignored.",atOp);
						}else{
							flags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_public){
						if(flags&Attribute::PRIVATE_BIT){
							log(Logger::LOG_WARNING,"'@(public)' is used in combination with @(private) and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_type){
						if(inverseFlags&Attribute::TYPE_ATTR_BIT){
							log(Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							flags |= Attribute::TYPE_ATTR_BIT;
						}
						if(flags&Attribute::INIT_BIT)
							log(Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
					}else {
						throwError("Invalid property: '"+name.toString()+'\'',atOp);
					}
				}
			}

		}
		StringId memberIdentifier;
		Object * obj=NULL;

		Object * indexExp=NULL;
		const int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=getExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;


		/// a:=2 => _.[a] := 2
		if (lValueType != LVALUE_MEMBER) {
			throwError("No valid member-LValue before '"+op->getString()+"' ",tokens[opPosition]);
		}
		if(obj==NULL){
			log(Logger::LOG_WARNING,"Use '=' for assigning to local variables instead of '"+op->getString()+"' ",tokens[opPosition]);
			return SetAttributeExpr::createAssignment(obj,memberIdentifier,rightExpression,currentLine);
		}
		return new SetAttributeExpr(obj,memberIdentifier,rightExpression,flags,currentLine);
	}


	/// get left expression
	Object * leftExpression=getExpression(ctxt,leftExprFrom,leftExprTo);

	/// "a.b.c"
	if (op->getString()==".") {
		if (rightExprFrom>to) {
			log(Logger::LOG_DEBUG, "Error .1 ",tokens[opPosition]);
			throwError("Syntax error after '.'",tokens[opPosition]);
		}
		cursor=to;

		/// "a.b"
		if (Token::isA<TIdentifier>(tokens[rightExprFrom])){
			return new GetAttributeExpr(leftExpression,Token::cast<TIdentifier>(tokens[rightExprFrom])->getId());
		}
		/// "a.+"
		else if (Token::isA<TOperator>(tokens[rightExprFrom])) {
			return new GetAttributeExpr(leftExpression,Token::cast<TOperator>(tokens[rightExprFrom])->toString());
		}
		else if(Token::isA<TObject>(tokens[rightExprFrom])){
			Object * obj=Token::cast<TObject>(tokens[rightExprFrom])->obj.get();
			/// "a.'+'"
			if (String * s=dynamic_cast<String *>(obj)) {
				return new GetAttributeExpr(leftExpression,s->toString());
			}/// "a.$b"
			else if (Identifier * i=dynamic_cast<Identifier *>(obj)) {
				return new GetAttributeExpr(leftExpression,i->getId());
			}
		}
		log(Logger::LOG_DEBUG, "Error .2 ",tokens[opPosition]);
		throwError("Syntax error after '.'",tokens[opPosition]);
	}
	///  Function Call
	/// "a(b)"  "a(1,2,3)"
	else if (op->getString()=="(") {
		cursor=rightExprFrom-1;
		std::vector<ObjRef> paramExp;
		getExpressionsInBrackets(ctxt,cursor,paramExp);

		if(cursor!=to){
			throwError("Error after function call. Forgotten ';' ?",tokens.at(cursor));
		}
		FunctionCallExpr * funcCall = new FunctionCallExpr(leftExpression,paramExp,false,currentFilename,currentLine);
		return funcCall;
	}
	///  Index Exression | Array
	else if (op->getString()=="[") {
		/// No left expression present? -> Array-constructor
		///"[1,a+2,3]" -> new Array(1,a+2,3)
		if (!leftExpression) {
			std::vector<ObjRef> paramExp;
			++cursor;
			while (!Token::isA<TEndIndex>(tokens.at(cursor)) ) {

				paramExp.push_back(getExpression(ctxt,cursor));

				++cursor;
				if (Token::isA<TDelimiter>(tokens.at(cursor)))
					++cursor;
				else if (!Token::isA<TEndIndex>(tokens.at(cursor))){
					throwError("Expected ]",tokens[opPosition]);
				}
			}
			cursor=to;

			FunctionCallExpr * funcCall = new FunctionCallExpr( Array::getTypeObject()->getAttribute(Consts::IDENTIFIER_fn_constructor).getValue(),
													paramExp,false,currentFilename,currentLine);
			return funcCall;
		}
		/// Left expression present? -> Index Expression
		/// "a[1]"
		cursor=rightExprFrom;
		std::vector<ObjRef> paramExp;
		paramExp.push_back(getExpression(ctxt,cursor));
		cursor=to;
		FunctionCallExpr * funcCall = new FunctionCallExpr(new GetAttributeExpr(leftExpression,Consts::IDENTIFIER_fn_get),paramExp,
										false,currentFilename,currentLine);
		return funcCall;

	}/// "a?1:2"
	else if (op->getString()=="?") {
		cursor=rightExprFrom;
		Object * alt1=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TColon>(tokens.at(cursor))) {
			throwError("Expected ':'",tokens.at(cursor));
		}
		++cursor;
		Object * alt2=getExpression(ctxt,cursor);
		return new ConditionalExpr(leftExpression,alt1,alt2);
	} /// new Object
	else if (op->getString()=="new") {
		++cursor;
		if (leftExpression)
			throwError("new is a unary left operator.",tokens.at(cursor));

		int objExprTo=to;

		/// if new has paramteres "(...)", search for their beginning.
		if (Token::isA<TEndBracket>(tokens[objExprTo])) {
			objExprTo=findCorrespondingBracket<TEndBracket,TStartBracket>(ctxt,objExprTo,rightExprFrom,-1);
		}
		/// read parameters
		std::vector<ObjRef> paramExp;
		if (objExprTo>cursor) {
			int cursor2=objExprTo;
			getExpressionsInBrackets(ctxt,cursor2,paramExp);

			objExprTo--; /// why ?????????????
		}
		/// read Object-expression
		Object * obj=getExpression(ctxt,cursor,objExprTo);
		cursor=to;

		return new FunctionCallExpr(new GetAttributeExpr(obj,Consts::IDENTIFIER_fn_constructor),paramExp,true,
									currentFilename,currentLine);
		// TODO: !!! Return this-reference !!! ???? What does this mean?
	}
	/// Function "fn(a,b){return a+b;}"
	else if (op->getString()=="fn" ){//|| op->getString()=="lambda") {
		ObjRef result=getFunctionDeclaration(ctxt,cursor);
		if (cursor!=to)    {
			throwError("[fn] Syntax error.",tokens.at(cursor));
		}
		return result.detachAndDecrease();
	}

	Object * rightExpression=getExpression(ctxt,rightExprFrom,to);

	cursor=rightExprFrom;

	/// Unary prefix expression
	/// ++a, --a, !a
	/// Bsp.: ++a =>  _.a.++pre()
	if (! leftExpression) {
		/// +a  +3
		if (op->getString()=="+"){
			// @optimization
			if (Number* num=dynamic_cast<Number*>(rightExpression)) {
				return num;
			}
		}
		/// -a  -3
		else if (op->getString()=="-") {
			// @optimization
			if (Number* num=dynamic_cast<Number*>(rightExpression)) {
				Number * newNum=Number::create(-num->toDouble());
				Number::release(num);
				return newNum;
			}
//            if (Number* num=dynamic_cast<Number*>(rightExpression)) {
//                Number * newNum=Number::create(-num->toDouble());
//                delete num;
//                return newNum ;
//            }
		} else if (op->getString()=="!") {
			return new LogicOpExpr(rightExpression,0,LogicOpExpr::NOT);
		}

		//if (GetAttributeExpr * ga=dynamic_cast<GetAttributeExpr *>(rightExpression)) {
		FunctionCallExpr * fc=new FunctionCallExpr(
			new GetAttributeExpr(rightExpression,
							 string(op->getString())+"_pre"),std::vector<ObjRef>(),false,currentFilename,currentLine);
		return  fc;

	} else
		/// Unary postfix expression
		/// a++, a--, a!
		/// Bsp: a++ => _.a.++post()
		if (!rightExpression) {
			//  if (GetAttributeExpr * ga=dynamic_cast<GetAttributeExpr *>(leftExpression)) {
			FunctionCallExpr * fc=new FunctionCallExpr(
				new GetAttributeExpr(leftExpression,
								 string(op->getString())+"_post"),std::vector<ObjRef>(),false,currentFilename,currentLine);
			cursor--;

			return  fc;
		}
	/// ||
		else if (op->getString()=="||") {
			return new LogicOpExpr(leftExpression,rightExpression,LogicOpExpr::OR);
		}
	/// &&
		else if (op->getString()=="&&") {
			return new LogicOpExpr(leftExpression,rightExpression,LogicOpExpr::AND);
		}
	/// normal binary expression
	/// 1+2 -> 1.+(2)
		else {
			std::vector<ObjRef> paramExp;
			paramExp.push_back(rightExpression);
			FunctionCallExpr * funcCall = new FunctionCallExpr(new GetAttributeExpr(leftExpression, op->getString()),paramExp,
													false,currentFilename,currentLine);
			return funcCall;
		}
	return NULL;
}

/*!	Read a function declaration. Must begin with "fn"
	Cursor is placed at the end of the block.
	\note after pass_2(...) a function looks like this:
			fn( (params*) {...} )  OR
			fn( (params*).(constrExpr) {...} )
			fn( (params*)@(super()) {...} )
	*/
Object * Parser::getFunctionDeclaration(ParsingContext & ctxt,int & cursor)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();

	if(t->toString()!="fn"){
		throwError("No function! ",tokens.at(cursor));
	}
	const size_t codeStartPos = t->getStartingPos();

	++cursor;

	/// step over '(' inserted at pass_2(...)
	++cursor;

	if(_produceBytecode){
		UserFunctionExpr::parameterList_t params;
		readFunctionParameters(params,ctxt,cursor);
		TOperator * superOp=Token::cast<TOperator>(tokens.at(cursor));

		/// fn(a).(a+1,2){} \deprecated
		std::vector<ObjRef> superConCallExpressions;
		if(superOp!=NULL && superOp->toString()=="."){
			++cursor;
			getExpressionsInBrackets(ctxt,cursor,superConCallExpressions);
			++cursor; // step over ')'

		} /// fn(a)@(super(a+1,2)) {}
		else if(superOp!=NULL && superOp->toString()=="@"){
			++cursor;
			if(!Token::isA<TStartBracket>(tokens.at(cursor))){
				throwError("Property expects brackets.",superOp);
			}
			const int propertyTo = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor);

			properties_t properties;
			readProperties(ctxt,cursor+1,propertyTo-1,properties);
			for(properties_t::const_iterator it=properties.begin();it!=properties.end();++it ){
				const StringId name = it->first;
				int parameterPos = it->second;
				log(Logger::LOG_INFO,"Property:"+name.toString(),superOp );
	//					const int pos = it->second;
				if(name == Consts::PROPERTY_FN_super){
					if(parameterPos<0){
						throwError("Super attribute needs parameter list.",superOp);
					}
					getExpressionsInBrackets(ctxt,parameterPos,superConCallExpressions);
				}else{
					log(Logger::LOG_WARNING,"Anntoation is invalid for functions: '"+name.toString()+"'",superOp);
				}
			}
			cursor = propertyTo+1;

		}


		ctxt.blocks.push_back(NULL); // mark beginning of new local namespace
		BlockStatement * block=dynamic_cast<BlockStatement*>(getExpression(ctxt,cursor));
		if (block==NULL) {
			throwError("[fn] Expects BlockStatement of statements.",tokens.at(cursor));
		}
		ctxt.blocks.pop_back(); // remove marking for local namespace

		const size_t codeEndPos = tokens.at(cursor)->getStartingPos(); // position of '}'

		/// step over ')' inserted at pass_2(...)
		++cursor;

		{	// create function expression
			UserFunctionExpr * uFunExpr = new UserFunctionExpr(block,superConCallExpressions);
			uFunExpr->getParamList().swap(params);	// set parameter expressions
			
			// store code segment in userFunction
			if(codeStartPos!=std::string::npos && codeEndPos!=std::string::npos && !ctxt.code.isNull()){
				uFunExpr->setCodeString(ctxt.code,codeStartPos,codeEndPos-codeStartPos+1);
			}
			return uFunExpr;
		}
	}else{
		UserFunction::parameterList_t * params=getFunctionParameters(ctxt,cursor);
		TOperator * superOp=Token::cast<TOperator>(tokens.at(cursor));

		/// fn(a).(a+1,2){} \deprecated
		std::vector<ObjRef> superConCallExpressions;
		if(superOp!=NULL && superOp->toString()=="."){
			++cursor;
			getExpressionsInBrackets(ctxt,cursor,superConCallExpressions);
			++cursor; // step over ')'

		} /// fn(a)@(super(a+1,2)) {}
		else if(superOp!=NULL && superOp->toString()=="@"){
			++cursor;
			if(!Token::isA<TStartBracket>(tokens.at(cursor))){
				throwError("Property expects brackets.",superOp);
			}
			const int propertyTo = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor);

			properties_t properties;
			readProperties(ctxt,cursor+1,propertyTo-1,properties);
			for(properties_t::const_iterator it=properties.begin();it!=properties.end();++it ){
				const StringId name = it->first;
				int parameterPos = it->second;
				log(Logger::LOG_INFO,"Property:"+name.toString(),superOp );
	//					const int pos = it->second;
				if(name == Consts::PROPERTY_FN_super){
					if(parameterPos<0){
						throwError("Super attribute needs parameter list.",superOp);
					}
					getExpressionsInBrackets(ctxt,parameterPos,superConCallExpressions);
				}else{
					log(Logger::LOG_WARNING,"Anntoation is invalid for functions: '"+name.toString()+"'",superOp);
				}
			}
			cursor = propertyTo+1;

		}


		ctxt.blocks.push_back(NULL); // mark beginning of new local namespace
		BlockStatement * block=dynamic_cast<BlockStatement*>(getExpression(ctxt,cursor));
		if (block==NULL) {
			throwError("[fn] Expects BlockStatement of statements.",tokens.at(cursor));
		}
		ctxt.blocks.pop_back(); // remove marking for local namespace

		size_t codeEndPos = tokens.at(cursor)->getStartingPos(); // position of '}'

		/// step over ')' inserted at pass_2(...)
		++cursor;

		UserFunction * uFun = new UserFunction(params,block,superConCallExpressions);
		// store code segment in userFunction
		if(codeStartPos!=std::string::npos && codeEndPos!=std::string::npos && !ctxt.code.isNull()){
			uFun->setCodeString(ctxt.code,codeStartPos,codeEndPos-codeStartPos+1);
		}
		return uFun;
	}
}


/**
 * Reads a Control-Statement from tokens beginning at index "cursor".
 * Cursor is placed at the last Token of the statement.
 * @param tokens Program as Token-List.
 * @param curosr Cursor pointing at current Token.
 * @return Control-statement or NULL if no Control-Statement could be read.
 */
Statement Parser::getControl(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TControl * tc=Token::cast<TControl>(tokens.at(cursor));
	if (!tc) return Statement(Statement::TYPE_UNDEFINED);
	++cursor;

	StringId cId=tc->getId();
	/// if-Control
	if(cId==Consts::IDENTIFIER_if){
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError("[if] expects (",tokens.at(cursor));
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError("[if] expects (...)",tokens.at(cursor));
		}
		++cursor;
		Statement action=getStatement(ctxt,cursor);
		Statement elseAction;
		if ((tc=Token::cast<TControl>(tokens.at(cursor+1)))) {
			if (tc->getId()==Consts::IDENTIFIER_else) {
				++cursor;
				++cursor;
				elseAction=getStatement(ctxt,cursor);
			}
		}
		return Statement(Statement::TYPE_IF,
							new IfStatement(condition,
												action,
												elseAction));
	}
	/// for-Control
	/*	for( [init] ; [condition] ; [incr] ) [action]

		{
			[init]
		A:
			if( [condition] )
				[action]
			else
				break;
		continue:
			[incr]
			goto A:
		} break;
	*/
	else if(cId==Consts::IDENTIFIER_for) {
		if (!Token::isA<TStartBlock>(tokens.at(cursor))) // for{...;...;...}
			throwError("[for] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockStatement * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Statement initExp=createStatement(getExpression(ctxt,cursor));
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError("[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError("[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Statement incr=createStatement(getExpression(ctxt,cursor));
		if (incr.isValid())
			++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError("[for] expects )",tokens.at(cursor));
		}
		++cursor;
		Statement action=getStatement(ctxt,cursor);

		if(_produceBytecode){
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, 
														LoopStatement::createForLoop(initExp,condition,incr,action) ) );
			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		}else{
			loopWrappingBlock->addStatement( initExp );
			loopWrappingBlock->setJumpPosA( loopWrappingBlock->getNextPos() );
			if(condition!=NULL)
				loopWrappingBlock->addStatement( Statement(
							Statement::TYPE_IF,
							new IfStatement(condition,action,Statement(Statement::TYPE_BREAK))));
			else if(action.isValid())
				loopWrappingBlock->addStatement(action);

			loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
			loopWrappingBlock->addStatement( incr );
			loopWrappingBlock->addStatement( Statement( Statement::TYPE_JUMP_TO_A ) );
			loopWrappingBlock->setBreakPos( BlockStatement::POS_HANDLE_AND_LEAVE );

			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		}
	}
	/// while-Control
	/*
		{
		continue:
		A:
			if( [condition] )
				[action]
			else
				break;
			goto A:
		} break:
	*/
	else if(cId==Consts::IDENTIFIER_while) {
		if (!Token::isA<TStartBlock>(tokens.at(cursor))) // while{...}
			throwError("[while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockStatement * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError("[while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		Statement action=getStatement(ctxt,cursor);
		if(_produceBytecode){
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, LoopStatement::createWhileLoop(condition,action) ) );
			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
			
		}else{
			loopWrappingBlock->setContinuePos( BlockStatement::POS_HANDLE_AND_RESTART );
			loopWrappingBlock->setJumpPosA( BlockStatement::POS_HANDLE_AND_RESTART );
			loopWrappingBlock->addStatement( Statement(
							Statement::TYPE_IF,
							new IfStatement(condition,action,Statement(Statement::TYPE_BREAK))));
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A));

			loopWrappingBlock->setBreakPos( BlockStatement::POS_HANDLE_AND_LEAVE );

			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		}
		
		
	}
	/// Do-while-Control
	/*
		{
		A:
			[action]
		continue:
			if( [condition] )
				goto A:
		} break:
	*/
	else if(cId==Consts::IDENTIFIER_do) {
		Statement action=getStatement(ctxt,cursor);
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_while)
			throwError("[do-while] expects while",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBlock>(tokens.at(cursor))) // do{} while{...};
			throwError("[do-while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockStatement * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError("[do-while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError("[do-while] expects ;",tokens.at(cursor));
		}
		if(_produceBytecode){
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, LoopStatement::createDoWhileLoop(condition,action) ) );
			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
			
		}else{
			loopWrappingBlock->setJumpPosA( BlockStatement::POS_HANDLE_AND_RESTART );
			loopWrappingBlock->addStatement( action );
			loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
			loopWrappingBlock->addStatement( Statement(
							Statement::TYPE_IF,
							new IfStatement(condition, Statement(Statement::TYPE_JUMP_TO_A),Statement())));
			loopWrappingBlock->setBreakPos( BlockStatement::POS_HANDLE_AND_LEAVE );

			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		}

	}
	/// foreach-Control
	/*	foreach( [array] as [keyIdent],[valueIndent] ) [action]

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
	

	else if(cId==Consts::IDENTIFIER_foreach) {
		if (!Token::isA<TStartBlock>(tokens.at(cursor)))  // foreach{...as...}
			throwError("[foreach] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockStatement * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * arrayExpression=getExpression(ctxt,cursor);
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_as)
			throwError("[foreach] expects as",tokens.at(cursor));
		++cursor;

		TIdentifier * valueIdent=NULL;
		TIdentifier * keyIdent=NULL;
		if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
			throwError("[foreach] expects Identifier-1",tokens.at(cursor));
		++cursor;

		if (Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			keyIdent=valueIdent;
			if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
				throwError("[foreach] expects Identifier-2",tokens.at(cursor));
			++cursor;
		}

		if (!Token::isA<TEndBlock>(tokens.at(cursor)))
			throwError("[foreach] expects )",tokens.at(cursor));
		++cursor;
		Statement action=getStatement(ctxt,cursor);

		if(_produceBytecode){
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, new ForeachStatement(
					arrayExpression,keyIdent?keyIdent->getId():StringId(), valueIdent?valueIdent->getId():StringId(), action) ) );
			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		}else{
			static const StringId itId("__id");
			static const StringId getIteratorId("getIterator");
			static const StringId keyFnId("key");
			static const StringId valueFnId("value");

			// var __it;
			loopWrappingBlock->declareVar(itId);

			// __it = __getIterator([array])
			struct fnWrapper {
				ES_FUNCTION(esf_getIterator){
					Object * it=NULL;
					if(	Collection * c=parameter[0].toType<Collection>()){
						it = c->getIterator();
					}else if(parameter[0].toType<YieldIterator>()){
						it = parameter[0].get();
					}else {
						it = callMemberFunction(runtime,parameter[0] ,getIteratorId,ParameterValues());
					}
					if(it!=NULL){
						runtime.assignToVariable(itId,it);
					}else{
						runtime.setException("Could not get iterator from '" + parameter[0]->toDbgString() + '\'');
						return NULL;
					}
					return NULL;
				}
				ES_FUNCTION(esf_setValues){
					ObjPtr  it=runtime.getVariable(itId);
					Identifier * valueId=parameter[0].toType<Identifier>();
					Identifier * keyId=parameter[1].toType<Identifier>();
					if(Iterator * it2=it.toType<Iterator>()){
						if(keyId!=NULL)
							runtime.assignToVariable(keyId->getId(),it2->key());
						runtime.assignToVariable(valueId->getId(),it2->value());
					}else if(YieldIterator * yIt=it.toType<YieldIterator>()){
						if(keyId!=NULL)
							runtime.assignToVariable(keyId->getId(),yIt->key());
						runtime.assignToVariable(valueId->getId(),yIt->value());
					}else{
						if(keyId!=NULL)
							runtime.assignToVariable(keyId->getId(),
										callMemberFunction(runtime,it ,keyFnId,ParameterValues()));
						runtime.assignToVariable(valueId->getId(),
									callMemberFunction(runtime,it ,valueFnId,ParameterValues()));
					}
					return NULL;
				}
			};
			{
				std::vector<ObjRef> paramExp;
				paramExp.push_back(arrayExpression);
				Object * getIterator = new FunctionCallExpr(new Function(fnWrapper::esf_getIterator), paramExp);
				loopWrappingBlock->addStatement( createStatement(getIterator));
			}

			loopWrappingBlock->setJumpPosA( loopWrappingBlock->getNextPos() );
			// if( __it.end() )	break;
			static const StringId endId("end");
			Object * condition = new FunctionCallExpr(
									new GetAttributeExpr(new GetAttributeExpr(NULL,itId),endId ),std::vector<ObjRef>());
			loopWrappingBlock->addStatement( Statement(
							Statement::TYPE_IF,
							new IfStatement(condition,Statement(Statement::TYPE_BREAK),Statement())));

			{ // key == __it.key(), value==__it.value()
				std::vector<ObjRef> paramExp;
				paramExp.push_back( Identifier::create(valueIdent->getId()));
				if(keyIdent!=NULL)
					paramExp.push_back( Identifier::create(keyIdent->getId()));
				loopWrappingBlock->addStatement( createStatement(new FunctionCallExpr(new Function(fnWrapper::esf_setValues), paramExp)));

			}
			// [action]
			loopWrappingBlock->addStatement( action );
			// :continue
			loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
			// __it.next();
			static const StringId nextFnId("next");
			loopWrappingBlock->addStatement( createStatement(
							new FunctionCallExpr(
								new GetAttributeExpr(new GetAttributeExpr(NULL,itId),nextFnId ),std::vector<ObjRef>())));
			// goto :second
			loopWrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A) );

			loopWrappingBlock->setBreakPos( BlockStatement::POS_HANDLE_AND_LEAVE );

			return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
		
		}
		
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
		BlockStatement * tryBlock=dynamic_cast<BlockStatement*>(getExpression(ctxt,cursor)); 
		if(!tryBlock){
			throwError("[try-catch] expects an try block. ",tokens.at(cursor));
		}
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_catch)
			throwError("[try-catch] expects catch",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError("[try-catch] expects (",tokens.at(cursor));
		++cursor;
		TIdentifier * tIdent=NULL;

		StringId varName;
		bool hasVarName=false;
		if ((tIdent=Token::cast<TIdentifier>(tokens.at(cursor)))) {
			++cursor;
			varName=tIdent->getId();
			hasVarName=true;
		}

		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError("[try-catch] expects ([Identifier])",tokens.at(cursor));
		}
		++cursor;

		TStartBlock * tStartCatchBlock = Token::cast<TStartBlock>(tokens.at(cursor));
		if(tStartCatchBlock==NULL){
			throwError("[catch] expects BlockStatement {...}",tokens.at(cursor));
		}

		BlockStatement * catchBlock=tStartCatchBlock->getBlock();
		if(_produceBytecode){
			getExpression(ctxt,cursor); // fill rest of catch block
			return Statement(Statement::TYPE_STATEMENT,new TryCatchStatement(tryBlock,catchBlock,varName));
		}else{
			if(hasVarName){
				catchBlock->declareVar(varName);
				std::vector<ObjRef> paramExp;

				struct fnWrapper {
					ES_FUNCTION(extractExceptionValue){
						ObjRef exceptionValue=runtime.getResult();
						runtime.resetState();
						return exceptionValue.detachAndDecrease();
					}
				};
				catchBlock->addStatement( Statement( Statement::TYPE_EXPRESSION,
						SetAttributeExpr::createAssignment(NULL,varName,
							new FunctionCallExpr(
								new Function(fnWrapper::extractExceptionValue),
								paramExp,false,currentFilename,tStartCatchBlock->getLine()),
							tStartCatchBlock->getLine())));

			}
			getExpression(ctxt,cursor); // fill rest of catch block

			BlockStatement * wrappingBlock = new BlockStatement();
			wrappingBlock->addStatement( createStatement(tryBlock) );
			wrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A) );
			wrappingBlock->setExceptionPos( wrappingBlock->getNextPos() );
			wrappingBlock->addStatement( Statement(Statement::TYPE_BLOCK,catchBlock) );
			wrappingBlock->setJumpPosA(  BlockStatement::POS_HANDLE_AND_LEAVE );
			return Statement(Statement::TYPE_BLOCK,wrappingBlock);
		}
	}
	/// continue-Control
	else if(cId==Consts::IDENTIFIER_continue) {
		return Statement(Statement::TYPE_CONTINUE);
	}
	/// break-Control
	else if(cId==Consts::IDENTIFIER_break) {
		return Statement(Statement::TYPE_BREAK);
	}
	/// return-Control
	else if(cId==Consts::IDENTIFIER_return) {
		return Statement(Statement::TYPE_RETURN,getExpression(ctxt,cursor));
	}
	/// exit-Control
	else if(cId==Consts::IDENTIFIER_exit) {
		return Statement(Statement::TYPE_EXIT,getExpression(ctxt,cursor));
	}
	/// throw-Control
	else if(cId==Consts::IDENTIFIER_throw) {
		return Statement(Statement::TYPE_EXCEPTION,getExpression(ctxt,cursor));
	}
	/// yield-Control
	else if(cId==Consts::IDENTIFIER_yield) {
		return Statement(Statement::TYPE_YIELD,getExpression(ctxt,cursor));
	}
	else{
		throwError(string("Parsing Unimplemented Control:")+tc->toString(),tokens.at(cursor));
		return Statement();
	}
}

//!	getLValue
Parser::lValue_t Parser::getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,
								StringId & identifier,Object * &indexExpression)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	/// Single Element: "a"
	if (to==from) {
		if (Token::isA<TIdentifier>(tokens[from])) {
			identifier=Token::cast<TIdentifier>(tokens.at(from))->getId();
			obj=NULL;
			return LVALUE_MEMBER;
//        }else if (Identifier * i=dynamic_cast<Identifier *>(tokens[from])) { // $a
//            identifier=i->getId();
//            obj=NULL;
//            return LVALUE_MEMBER;
		} else {
			throwError("LValue Error 1",tokens[from]);
		}
	}
	/// ".a"
	/// "a.b.c"
	if (Token::isA<TIdentifier>(tokens[to]) && Token::isA<TOperator>(tokens[to-1]) ) {
		if ( Token::cast<TOperator>(tokens.at(to-1))->getOperator()->getString()==".") {
			obj=getExpression(ctxt,from,to-2);
			identifier=Token::cast<TIdentifier>(tokens[to])->getId();
			return LVALUE_MEMBER;
		}
	}
// !!!!!!!!!!!!!!!!!!!!!!!!!! \todo ..............................
	if(TObject * tObj=Token::cast<TObject>(tokens[to])){
		/// ".'a'"
		/// "a.b.'c'"
		if (String * s=dynamic_cast<String *>(tObj->obj.get())) {
			TOperator * top=Token::cast<TOperator>(tokens[to-1]);

			if (top && top->getOperator()->getString()==".") {
				obj=getExpression(ctxt,from,to-2);
				identifier = s->toString();
				return LVALUE_MEMBER;
			}
		}
		/// ".$a"
		/// "a.b.$c"
		if (Identifier * i=dynamic_cast<Identifier *>(tObj->obj.get())) {
			TOperator * top=Token::cast<TOperator>(tokens[to-1]);

			if (top && top->getOperator()->getString()==".") {
				obj=getExpression(ctxt,from,to-2);
				identifier=i->getId();
				return LVALUE_MEMBER;
			}
		}
	}
	/// Index "a[1]"
	/// [a,b,c] //TODO!?
	if (Token::isA<TEndIndex>(tokens[to])) {

		int indexOpenPos=findCorrespondingBracket<TEndIndex,TStartIndex>(ctxt,to,from,-1);
		/// a[1]
		if (indexOpenPos>from) {
			obj=getExpression(ctxt,from,indexOpenPos-1);
			indexOpenPos++;
			indexExpression=getExpression(ctxt,indexOpenPos,to-1);
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
	if (Token::isA<TEndScript>(tokens.at(cursor)))
		return 0;

	int level=0;
	int to=cursor-1;
	int lastIdentifier=-10;

	Token * t=NULL;
	while (true) {
		to++;
		t=tokens.at(to).get();

		switch(t->getType()){
			case TStartBracket::TYPE_ID:{
				TStartBracket * sb=Token::cast<TStartBracket>(t);
				if(sb->endBracketIndex>1){
					to=sb->endBracketIndex;
				}else {
					level++;
				}
				continue;
			}
			case TStartBlock::TYPE_ID:
			case TStartMap::TYPE_ID:
			case TStartIndex::TYPE_ID:{
				level++;
				continue;
			}
			case TEndBlock::TYPE_ID:
			case TEndBracket::TYPE_ID:
			case TEndMap::TYPE_ID:
			case TEndIndex::TYPE_ID:{
				level--;

				if (level<0) {
					to--;
					return to;
				}
				continue;
			}
			case TEndScript::TYPE_ID:{
				if (level==1)
					return to;

				throwError("Unexpected Ending.",tokens.at(cursor));
			}
//
			default:{
			}
		}
		if (level>0)
			continue;
		switch(t->getType()){
			case TControl::TYPE_ID: {
				if (Token::cast<TControl>(t)->getId()==Consts::IDENTIFIER_as) {
					to--;
					return to;
				}
				throwError("Expressions cannot contain control statements.",t);
			}
			case TEndCommand::TYPE_ID:{
				return to;
			}
			case TDelimiter::TYPE_ID:
			case TMapDelimiter::TYPE_ID:
			case TColon::TYPE_ID:{
				to--;
				return to;
			}
			case TIdentifier::TYPE_ID:{
				if(lastIdentifier==to-1){
					to--;
					return to;
				}
				lastIdentifier=to;
				continue;
			}
			default:{
			}
		}
	}
	return to;
}
/**
 * e.g. (a, Number b, c=2+3)
 * Cursor is moved after the Parameter-List.
 */
UserFunction::parameterList_t * Parser::getFunctionParameters(ParsingContext & ctxt,int & cursor)const  {

	UserFunction::parameterList_t * params = new UserFunction::parameterList_t();

	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<TStartBracket>(tokens.at(cursor))) {
		return params;
	}
	++cursor;
	// fn (bla,blub,)
	bool first=true;

	while (true) { // foreach parameter
		if (first&&Token::isA<TEndBracket>(tokens.at(cursor))) {
			++cursor;
			break;
		}
		first=false;

		/// Parameter::= Expression? Identifier ( ('=' Expression)? ',') | ('*'? ('=' Expression)? ')')
		int c=cursor;

		// find identifierName, its position, the default expression and identify a multiParam
		int idPos=-1;
		StringId name;
		Object * defaultExpression=NULL;
		bool multiParam=false;
		while(true){
			Token * t=tokens.at(c).get();
			if(Token::isA<TIdentifier>(t)) {
				// this may not be the final identifier...
				name=Token::cast<TIdentifier>(t)->getId();
				idPos=c;

				Token * tNext=tokens.at(c+1).get();
				// '*'?
				if(  Token::isA<TOperator>(tNext) && tNext->toString()=="*" ){
					multiParam=true;
					++c;
					tNext=tokens.at(c+1).get();
				}else{
					multiParam=false;
				}
				// ',' | ')'
				if( Token::isA<TEndBracket>(tNext)){
					break;
				}else if( Token::isA<TDelimiter>(tNext) ) {
					if(multiParam)
						throwError("[fn] Only the last parameter may be a multiparameter.",tokens[c]);
					break;
				}else if(  Token::isA<TOperator>(tNext) && tNext->toString()=="=" ){
					int defaultExpStart=c+2;
					int defaultExpTo=findExpression(ctxt,defaultExpStart);
					defaultExpression=getExpression(ctxt,defaultExpStart,defaultExpTo);
					if (defaultExpression==NULL) {
						throwError("[fn] SyntaxError in default parameter.",tokens.at(cursor));
					}
					c=defaultExpTo;
					break;
				}
			}else if(Token::isA<TStartBracket>(t)){
				c=findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,c);
			}else if(Token::isA<TStartIndex>(t)){
				c=findCorrespondingBracket<TStartIndex,TEndIndex>(ctxt,c);
			}else if(Token::isA<TStartMap>(t)){
				c=findCorrespondingBracket<TStartMap,TEndMap>(ctxt,c);
			}else if(Token::isA<TEndScript>(t) || Token::isA<TEndBracket>(t)){
				throwError("[fn] Error in parameter definition.",t);
			}
			++c;
		}

		// get the type expression
		Object * typeExp=NULL;
		if(	idPos>cursor ){
			int tmpCursor=cursor;
			typeExp=getExpression(ctxt,tmpCursor,idPos-1);
		}

		// check if this is the last parameter
		bool lastParam=false;
		if(Token::isA<TEndBracket>(tokens[c+1])){
		lastParam=true;
		}else if( ! Token::isA<TDelimiter>(tokens[c+1])){
			throwError("[fn] SyntaxError.",tokens[c+1]);
		}

		// move cursor
		cursor=c+2;

		// create parameter
		UserFunction::Parameter * p=new UserFunction::Parameter(name,NULL,typeExp);
		if(multiParam)
			p->setMultiParam(true);
		if(defaultExpression!=NULL)
			p->setDefaultValueExpression(defaultExpression);
		params->push_back(p);
		if(lastParam){
			break;
		}
	}
	return params;
}
/**
 * e.g. (a, Number b, c=2+3)
 * Cursor is moved after the Parameter-List.
 */
void Parser::readFunctionParameters(UserFunctionExpr::parameterList_t & params,ParsingContext & ctxt,int & cursor)const  {
	params.clear();
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<TStartBracket>(tokens.at(cursor))) {
		return;
	}
	++cursor;
	// fn (bla,blub,)
	bool first=true;

	while (true) { // foreach parameter
		if (first&&Token::isA<TEndBracket>(tokens.at(cursor))) {
			++cursor;
			break;
		}
		first=false;

		/// Parameter::= Expression? Identifier ( ('=' Expression)? ',') | ('*'? ('=' Expression)? ')')
		int c=cursor;

		// find identifierName, its position, the default expression and identify a multiParam
		int idPos=-1;
		StringId name;
		Object * defaultExpression=NULL;
		bool multiParam=false;
		while(true){
			Token * t=tokens.at(c).get();
			if(Token::isA<TIdentifier>(t)) {
				// this may not be the final identifier...
				name=Token::cast<TIdentifier>(t)->getId();
				idPos=c;

				Token * tNext=tokens.at(c+1).get();
				// '*'?
				if(  Token::isA<TOperator>(tNext) && tNext->toString()=="*" ){
					multiParam=true;
					++c;
					tNext=tokens.at(c+1).get();
				}else{
					multiParam=false;
				}
				// ',' | ')'
				if( Token::isA<TEndBracket>(tNext)){
					break;
				}else if( Token::isA<TDelimiter>(tNext) ) {
					if(multiParam)
						throwError("[fn] Only the last parameter may be a multiparameter.",tokens[c]);
					break;
				}else if(  Token::isA<TOperator>(tNext) && tNext->toString()=="=" ){
					int defaultExpStart=c+2;
					int defaultExpTo=findExpression(ctxt,defaultExpStart);
					defaultExpression=getExpression(ctxt,defaultExpStart,defaultExpTo);
					if (defaultExpression==NULL) {
						throwError("[fn] SyntaxError in default parameter.",tokens.at(cursor));
					}
					c=defaultExpTo;
					break;
				}
			}else if(Token::isA<TStartBracket>(t)){
				c=findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,c);
			}else if(Token::isA<TStartIndex>(t)){
				c=findCorrespondingBracket<TStartIndex,TEndIndex>(ctxt,c);
			}else if(Token::isA<TStartMap>(t)){
				c=findCorrespondingBracket<TStartMap,TEndMap>(ctxt,c);
			}else if(Token::isA<TEndScript>(t) || Token::isA<TEndBracket>(t)){
				throwError("[fn] Error in parameter definition.",t);
			}
			++c;
		}

		// get the type expression
		Object * typeExp=NULL;
		if(	idPos>cursor ){
			int tmpCursor=cursor;
			typeExp=getExpression(ctxt,tmpCursor,idPos-1);
		}

		// check if this is the last parameter
		bool lastParam=false;
		if(Token::isA<TEndBracket>(tokens[c+1])){
		lastParam=true;
		}else if( ! Token::isA<TDelimiter>(tokens[c+1])){
			throwError("[fn] SyntaxError.",tokens[c+1]);
		}

		// move cursor
		cursor=c+2;

		// create parameter
		params.push_back(UserFunctionExpr::Parameter(name,NULL,typeExp));
		if(multiParam)
			params.back().setMultiParam(true);
		if(defaultExpression!=NULL)
			params.back().setDefaultValueExpression(defaultExpression);

		if(lastParam){
			break;
		}
	}
}

/*!	1,bla+2,(3*3)
	Cursor is moved at closing bracket ')'
*/
void Parser::getExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();
	if(t->toString()!="(") {
		throwError("Expression list error.",t);
	}
	++cursor;

	while (!Token::isA<TEndBracket>(tokens.at(cursor))) {
		if(Token::isA<TDelimiter>(tokens.at(cursor))){ // empty expression (1,,2)
			expressions.push_back(NULL);
			++cursor;
			continue;
		}
		expressions.push_back(getExpression(ctxt,cursor));
		++cursor;
		if (Token::isA<TDelimiter>(tokens.at(cursor))){
			++cursor;
		}else if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError("Expected )",tokens.at(cursor));
		}
	}
}

/**
 * A.m @(const,private,somthingWithOptions("foo")) := ...
 *       ^from                            ^p    ^to
 * ---> [ ($const,-1),($private,-1),($somthingWithOptions,p) ]
 */
void Parser::readProperties(ParsingContext & ctxt,int from,int to,properties_t & properties)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	for(int cursor = from;cursor<=to;++cursor){
		Token * t = tokens.at(cursor).get();
		const TIdentifier * tid = Token::cast<TIdentifier>(t);
		if( tid==NULL )
			throwError("Identifier expected in property",t);

		int optionPos = -1;
		++cursor;
		if( Token::isA<TStartBracket>(tokens.at(cursor)) && cursor < to ){ // property has options 'property(exp1,exp2)'
			optionPos = cursor;
			cursor = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to); // skip expressions in brackets
			if(cursor<0)
				throwError("Unclosed option list in property",tokens.at(cursor));
			++cursor; // skip ')'
		}
		if(cursor<=to && !Token::isA<TDelimiter>(tokens.at(cursor))){ // expect a delimiter or the end.
			throwError("Syntax error in property",tokens.at(cursor));
		}
		properties.push_back( std::make_pair(tid->getId(),optionPos) );
	}
}


void Parser::throwError(const std::string & msg,Token * token)const{
	ParserException * e = new ParserException(msg,token);
	e->setFilename(getCurrentFilename());
	throw e;
}

}
