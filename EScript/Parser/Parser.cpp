// Parser.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Parser.h"
#include <iostream>
#include <stdio.h>
#include <stack>
#include "Tokenizer.h"
#include "Operators.h"
#include "../EScript.h"
#include "../Objects/Internals/FunctionCall.h"
#include "../Objects/Internals/GetAttribute.h"
#include "../Objects/Internals/SetAttribute.h"
#include "../Objects/Internals/IfControl.h"
#include "../Objects/Internals/ConditionalExpr.h"
#include "../Objects/Internals/LogicOp.h"
#include "../Objects/Internals/Statement.h"

#include "../Objects/Number.h"
#include "../Objects/String.h"
#include "../Objects/UserFunction.h"
#include "../Objects/YieldIterator.h"
#include "../Objects/Identifier.h"

#include "../Utils/FileUtils.h"

namespace EScript {
using std::string;
// -------------------------------------------------------------------------------------------------------------------
// helper

template<class BracketStart,class BracketEnd>
int findCorrespondingBracket(const Parser::ParsingContext & ctxt,int from,int to=-1,int direction=1){
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<BracketStart>(tokens.at(from))){
		std::cout << __FILE__<<":"<<__LINE__<<" should not happen!";
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
Type* Parser::typeObject=NULL;


//! (static) initMembers
void Parser::init(EScript::Namespace & globals) {
//
	// Parser ---|> [Object]
	typeObject=new Type(Object::getTypeObject());
	declareConstant(&globals,getClassName(),typeObject);

	//!	[ESMF] Parser new Parser();
	ESF_DECLARE(typeObject,"_constructor",0,0,new Parser())

	//!	[ESMF] Block Parser.parse(String)
	ES_MFUNCTION_DECLARE(typeObject,Parser,"parse",1,1,{
		ERef<Block> blockRef(new Block());
		try {
			self->parse(blockRef.get(),parameter[0]->toString().c_str());
		} catch (Object * e) {
			runtime.error("",e);
			return NULL;
		}
		return blockRef.detachAndDecrease();
	})

	//!	[ESMF] Block Parser.parseFile(String filename)
	ES_MFUNCTION_DECLARE(typeObject,Parser,"parseFile",1,1, {
		ERef<Block> blockRef=new Block();
		blockRef->setFilename(stringToIdentifierId(parameter[0]->toString()));
		try {
			self->parseFile(blockRef.get(),parameter[0]->toString().c_str());
		} catch (Object * e) {
			runtime.error("",e);
			return NULL;
		}
		return blockRef.detachAndDecrease();
	})
}

// -------------------------------------------------------------------------------------------------------------------

//!	(ctor)
Parser::Parser(Type * type):Object(type?type:typeObject) {
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


/**
 *  Loads and parses a File.
 */
Object *  Parser::parseFile(Block * rootBlock,const std::string & filename)throw(Exception *) {
	size_t size=0;
	char * buffer=NULL;
	buffer=FileUtils::loadFile(filename,size);
	if (buffer==NULL)
		throw new Error(string("Could not open file: '")+filename+"'");

	tokenizer.defineToken("__FILE__",new TObject(String::create(filename)));
	tokenizer.defineToken("__DIR__",new TObject(String::create(FileUtils::dirname(filename))));

	currentFilename=stringToIdentifierId(filename);

	//    cout << buffer;
	Object *  s=NULL;
	try {
		s= parse(rootBlock,buffer);
	} catch (Exception * e) {
		e->setMessage(e->getMessage()+" in file '"+filename+"'");
		delete [] buffer;
		throw(e);
	} catch (...) {
		std::cout << "!!!!";
	}

	delete [] buffer;
	return s;
}

/**
 *  Parse a CString.
 */
Object *  Parser::parse(Block * rootBlock,const char * c)throw(Exception *) {

	Tokenizer::tokenList_t tokens;
	ParsingContext ctxt(tokens,String::create(c));
	ctxt.rootBlock=rootBlock;

	/// 1. Tokenize
	try {
		tokenizer.getTokens(c,tokens);
		pass_1(ctxt);
	} catch (Exception * e) {
		//std::cerr << e->toString() << std::endl;
		throw e;
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

/**
 * [Helper]
 */
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
 * - disambiguate Map/Block
 * - colon ( Mapdelimiter / shortIf ?:)
 */
void Parser::pass_1(ParsingContext & ctxt)throw(Exception *) {
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
					throw new Error("Syntax Error: ')'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndIndex::TYPE_ID:{
				if (!Token::isA<TStartIndex>(cbi.token)) {
					throw new Error("Syntax Error: ']'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndBlock::TYPE_ID:{
				if (!cbi.isCommandBlock) {
					throw new Error("Syntax Error: '}'",token);
				}
				/// Block is Map Constructor
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
					throw new Error("Syntax Error in Block: ':'",token);
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
					throw new Error("Syntax Error in Map: ';'",token);
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
		throw new Error("Unexpected eof (unclosed '"+bInfStack.top().token->toString()+"'?)",bInfStack.top().token);
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
					Tokenizer::tokenList_t & enrichedTokens)const throw(Exception *) {

	std::stack<Block *> blockStack;
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
				/// Variable Declaration
				TControl * tc=Token::cast<TControl>(token);
				if (tc->getId()==Consts::IDENTIFIER_var) {
					if (TIdentifier * ti=Token::cast<TIdentifier>(ctxt.tokens.at(cursor+1))) {
						if(!blockStack.top()->declareVar(ti->getId())){
							std::cout << "\n Warning: Duplicate local variable '"<<ti->toString()<<"' ("<<getCurrentFilename()<<":"<<ti->getLine()<<")\n";
						}
//						Token::removeReference(token);
						continue;
					} else
						throw new Error("var expects Identifier.",tc);
				}
				/// for(...) ---> for{...}
				else if(tc->getId()==Consts::IDENTIFIER_for || tc->getId()==Consts::IDENTIFIER_foreach || tc->getId()==Consts::IDENTIFIER_while){
					if( ctxt.tokens.at(cursor+1)->getType()!=TStartBracket::TYPE_ID )
						throw new Error(tc->toString()+" expects '('",tc);
					int endPos = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor+1);
					if(endPos<0)
						throw new Error("Error in loop condition",tc);
					loopConditionEndingBrackets.push(ctxt.tokens.at(endPos).get());

					enrichedTokens.push_back(token);
					++cursor;
					Block * loopConditionBlock=new Block(tc->getLine());
					loopConditionBlock->setFilename(currentFilename); /// debugging informations:
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
			/// Open new Block
			case TStartBlock::TYPE_ID:{
				TStartBlock * sb=Token::cast<TStartBlock>(token);
				Block * currentBlock=new Block(sb->getLine());//currentBlock);

				/// debugging informations:
				currentBlock->setFilename(currentFilename);

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
				if (blockStack.empty())
					throw new Error("Unexpected }");

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
					throw new Error("Missing opening bracket for ",token);

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
					throw new Error("Unclosed {");

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
Object * Parser::getExpression(ParsingContext & ctxt,int & cursor,int to)const throw(Exception *) {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (cursor>=static_cast<int>(tokens.size())){
		return NULL;
	}/// Commands: if(...){}
	else if (Token::isA<TControl>(tokens.at(cursor))) {
		std::cout << "No control here!";
		return NULL;
	} /// Block: {...}
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
				Block * b=ctxt.blocks.at(i);
				if(b==NULL)
					break;
				 else if(b->isLocalVar(ident->getId())){
//////					std::cout <<"local:"<<ident->toString()<<"\n";
					break;
				 }
		}
			return new GetAttribute(NULL,ident->getId());  // ID
		}
		std::cout << t->getLine()<<"\n";
		throw new Error("Unknown (or unimplemented) Token",t);
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
		std::cout << "\n Error "<<cursor<<" - "<<to<<" :";
		for (;cursor<=to;++cursor) {
			std::cout << tokens.at(cursor)->toString();
			// TODO:LINE!!!
		}
		throw new Error("Syntax error",tokens.at(cursor).get());
	}
}

//! (internal)
Statement Parser::createStatement(Object *exp)const{
	if(exp==NULL)
		return Statement(Statement::TYPE_UNDEFINED);
	if( dynamic_cast<Block*>(exp) ){
		return Statement(Statement::TYPE_BLOCK,exp);
	}else if( dynamic_cast<IfControl*>(exp) ){
		return Statement(Statement::TYPE_IF,exp);
	}
	return Statement(Statement::TYPE_EXPRESSION,exp);
}

/*! (internal) */
Statement Parser::getStatement(ParsingContext & ctxt,int & cursor,int to)const throw (Exception *){

	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (Token::isA<TControl>(tokens.at(cursor))) {
		return getControl(ctxt,cursor);
	} /// sub-Block: {...}
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
Block * Parser::getBlock(ParsingContext & ctxt,int & cursor)const throw (Exception *) {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TStartBlock * tsb=Token::cast<TStartBlock>(tokens.at(cursor));
	Block * b=tsb?reinterpret_cast<Block *>(tsb->getBlock()):NULL;
	if (b==NULL)
		throw new Error("No Block!",tokens.at(cursor));

	ctxt.blocks.push_back(b);

	++cursor;


	/// Read commands.
	while (!Token::isA<TEndBlock>(tokens.at(cursor))) {
		if (Token::isA<TEndScript>(tokens.at(cursor)))
			throw new Error("Unclosed Block {...",tsb);

		int line=tokens.at(cursor)->getLine();
		Statement stmt=getStatement(ctxt,cursor);

		if(stmt.isValid()){
			stmt.setLine(line);
			b->addStatement(stmt);
		}

		/// Commands have to end on ";" or "}".
		if (!(Token::isA<TEndCommand>(tokens.at(cursor)) || Token::isA<TEndBlock>(tokens.at(cursor)))) {
			std::cout << tokens.at(cursor)->toString();
			throw new Error("Syntax Error in Block.",tokens.at(cursor));
		}
		++cursor;
	}
	ctxt.blocks.pop_back();
	return b;
}

/*!	getMap */
Object * Parser::getMap(ParsingContext & ctxt,int & cursor)const throw(Exception *) {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<TStartMap>(tokens.at(cursor)))
		throw new Error("No Map!",tokens.at(cursor));

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
			std::cout << tokens.at(cursor)->toString();
			throw new Error("Map: Expected : ",tokens.at(cursor));
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
			throw new Error("Map Syntax Error",tokens.at(cursor));
	}
//    FunctionCall * funcCall = new FunctionCall(
//					new GetAttribute(new GetAttribute(0,stringToIdentifierId("Map"),true),Consts::IDENTIFIER_fn_constructor),
//					paramExp,false,currentFilename,currentLine);

	FunctionCall * funcCall = new FunctionCall(
					Map::typeObject->getAttribute(Consts::IDENTIFIER_fn_constructor),
					paramExp,false,currentFilename,currentLine);
	return funcCall;
}

/*!	Binary expression	*/
Object * Parser::getBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const throw(Exception *) {
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
				throw new Error("Error in binary expression",t);
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
		identifierId memberIdentifier;
		Object * obj=NULL;
		Object * indexExp=NULL;
		int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=getExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;


		/// a=2 => _.[a] = 2
		if (lValueType== LVALUE_MEMBER) {
			return new SetAttribute(obj,memberIdentifier,rightExpression,SetAttribute::ASSIGN,currentLine);
		}
		/// a[1]=2 =>  _.a._set(1, 2)
		else if (lValueType == LVALUE_INDEX) {
			std::vector<ObjRef> paramExp;
			paramExp.push_back(indexExp);
			paramExp.push_back(rightExpression);
			return new FunctionCall(new GetAttribute(obj,Consts::IDENTIFIER_fn_set),paramExp,false,currentFilename,currentLine);
		} else {
			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throw new Error("Syntax error before '=' ",tokens[opPosition]);
		}
	} else if (op->getString()==":=") {
		identifierId memberIdentifier;
		Object * obj=NULL;
		Object * indexExp=NULL;
		int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=getExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;


		/// a:=2 => _.[a] := 2
		if (lValueType== LVALUE_MEMBER) {
		if(obj==NULL){
				std::cout << "\n Warning: ':=' used for assigning to non member variable; use '=' instead! ("<<
						getCurrentFilename()<<":"<<currentLine<<")\n";//<<tokens.at(cursor)->getLine()<<")";
		}
			return new SetAttribute(obj,memberIdentifier,rightExpression,SetAttribute::SET_OBJ_ATTRIBUTE,currentLine);
		}
//        // a[1]=2 =>  _.a._set(1, 2)
//        else if (lValueType == LVALUE_INDEX) {
//            std::vector<Object *> * paramExp=new  std::vector<Object *>();
//            paramExp->push_back(indexExp);
//            paramExp->push_back(rightExpression);
//            return new FunctionCall(new GetAttribute(obj,Consts::IDENTIFIER_fn__set ),paramExp,false,currentLine);
//        }
		else {
			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throw new Error("Syntax error before ':=' ",tokens[opPosition]);
		}
	} else if (op->getString()=="::=") {
		identifierId memberIdentifier;
		Object * obj=NULL;
		Object * indexExp=NULL;
		int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=getExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;

		/// a::=2 => _.[a] ::= 2
		if (lValueType== LVALUE_MEMBER) {
			return new SetAttribute(obj,memberIdentifier,rightExpression,SetAttribute::SET_TYPE_ATTRIBUTE,currentLine);
		}
		else {
			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throw new Error("Syntax error before '::=' ",tokens[opPosition]);
		}
	}

	/// get left expression
	Object * leftExpression=getExpression(ctxt,leftExprFrom,leftExprTo);

	/// "a.b.c"
	if (op->getString()==".") {
		if (rightExprFrom>to) {
			std::cout << "\n Error .1 "<<cursor<<" - "<<to<<" :";
			throw new Error("Syntax error after '.'",tokens[opPosition]);
		}
		cursor=to;

		/// "a.b"
		if (Token::isA<TIdentifier>(tokens[rightExprFrom])){
			return new GetAttribute(leftExpression,Token::cast<TIdentifier>(tokens[rightExprFrom])->getId());
		}
		/// "a.+"
		else if (Token::isA<TOperator>(tokens[rightExprFrom])) {
			return new GetAttribute(leftExpression,Token::cast<TOperator>(tokens[rightExprFrom])->toString());
		}
		else if(Token::isA<TObject>(tokens[rightExprFrom])){
			Object * obj=Token::cast<TObject>(tokens[rightExprFrom])->obj.get();
			/// "a.'+'"
			if (String * s=dynamic_cast<String *>(obj)) {
				return new GetAttribute(leftExpression,s->toString());
			}/// "a.$b"
			else if (Identifier * i=dynamic_cast<Identifier *>(obj)) {
				return new GetAttribute(leftExpression,i->getId());
			}
		}
		std::cout << "\n Error .2 "<<cursor<<" - "<<to<<" :";
		throw new Error("Syntax error after '.'",tokens[opPosition]);
	}
	///  Function Call
	/// "a(b)"  "a(1,2,3)"
	else if (op->getString()=="(") {
		cursor=rightExprFrom-1;
		std::vector<ObjRef> paramExp;
		getExpressionsInBrackets(ctxt,cursor,paramExp);

		if(cursor!=to){
			throw new Error("Error after function call. Forgotten ';' ?",tokens.at(cursor));
		}
		FunctionCall * funcCall = new FunctionCall(leftExpression,paramExp,false,currentFilename,currentLine);
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
					std::cout << tokens.at(cursor)->toString();
					throw new Error("Expected ]",tokens[opPosition]);
				}
			}
			cursor=to;
//            FunctionCall * funcCall = new FunctionCall(new GetAttribute(new GetAttribute(NULL,"Array",true),Consts::IDENTIFIER_fn_constructor),
//													paramExp,false,currentFilename,currentLine);
			FunctionCall * funcCall = new FunctionCall( Array::typeObject->getAttribute(Consts::IDENTIFIER_fn_constructor),
													paramExp,false,currentFilename,currentLine);
			return funcCall;
		}
		/// Left expression present? -> Index Expression
		/// "a[1]"
		cursor=rightExprFrom;
		std::vector<ObjRef> paramExp;
		paramExp.push_back(getExpression(ctxt,cursor));
		cursor=to;
		FunctionCall * funcCall = new FunctionCall(new GetAttribute(leftExpression,Consts::IDENTIFIER_fn_get),paramExp,
										false,currentFilename,currentLine);
		return funcCall;

	}/// "a?1:2"
	else if (op->getString()=="?") {
		cursor=rightExprFrom;
		Object * alt1=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TColon>(tokens.at(cursor))) {
			std::cout <<  tokens.at(cursor)->toString();
			throw new Error("Expected :",tokens.at(cursor));
		}
		++cursor;
		Object * alt2=getExpression(ctxt,cursor);
		return new ConditionalExpr(leftExpression,alt1,alt2);
	} /// new Object
	else if (op->getString()=="new") {
		++cursor;
		if (leftExpression)
			throw new Error("new is a unary left operator.",tokens.at(cursor));

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

		return new FunctionCall(new GetAttribute(obj,Consts::IDENTIFIER_fn_constructor),paramExp,true,
									currentFilename,currentLine);
		// TODO: !!! Return this-reference !!! ???? What does this mean?
	}
	/// Function "fn(a,b){return a+b;}"
	else if (op->getString()=="fn" ){//|| op->getString()=="lambda") {
		ObjRef result=getFunctionDeclaration(ctxt,cursor);
		if (cursor!=to)    {
			throw new Error("[fn] Syntax error.",tokens.at(cursor));
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
				delete num;
				return newNum ;
			}
//            if (Number* num=dynamic_cast<Number*>(rightExpression)) {
//                Number * newNum=Number::create(-num->toDouble());
//                delete num;
//                return newNum ;
//            }
		} else if (op->getString()=="!") {
			return new LogicOp(rightExpression,0,LogicOp::NOT);
		}

		//if (GetAttribute * ga=dynamic_cast<GetAttribute *>(rightExpression)) {
		FunctionCall * fc=new FunctionCall(
			new GetAttribute(rightExpression,
							 string(op->getString())+"_pre"),std::vector<ObjRef>(),false,currentFilename,currentLine);
		return  fc;

	} else
		/// Unary postfix expression
		/// a++, a--, a!
		/// Bsp: a++ => _.a.++post()
		if (!rightExpression) {
			//  if (GetAttribute * ga=dynamic_cast<GetAttribute *>(leftExpression)) {
			FunctionCall * fc=new FunctionCall(
				new GetAttribute(leftExpression,
								 string(op->getString())+"_post"),std::vector<ObjRef>(),false,currentFilename,currentLine);
			cursor--;

			return  fc;
		}
	/// ||
		else if (op->getString()=="||") {
			return new LogicOp(leftExpression,rightExpression,LogicOp::OR);
		}
	/// &&
		else if (op->getString()=="&&") {
			return new LogicOp(leftExpression,rightExpression,LogicOp::AND);
		}
	/// normal binary expression
	/// 1+2 -> 1.+(2)
		else {
			std::vector<ObjRef> paramExp;
			paramExp.push_back(rightExpression);
			FunctionCall * funcCall = new FunctionCall(new GetAttribute(leftExpression, op->getString()),paramExp,
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
	*/
Object * Parser::getFunctionDeclaration(ParsingContext & ctxt,int & cursor)const throw (Exception *){
//	bool lambda=false;
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();

//	if(t->toString()=="lambda"){
//		lambda=true;
//	}else
	if(t->toString()!="fn"){
		throw new Error("No function! ",tokens.at(cursor));
	}
	size_t codeStartPos = t->getStartingPos();
	
	++cursor;

	/// step over '(' inserted at pass_2(...)
	++cursor;

	UserFunction::parameterList_t * params=getFunctionParameters(ctxt,cursor);
	TOperator * superOp=Token::cast<TOperator>(tokens.at(cursor));

	/// fn(a).(a+1,2){}
	std::vector<ObjRef> superConCallExpressions;
	if(superOp!=NULL && superOp->toString()=="."){
		++cursor;
		getExpressionsInBrackets(ctxt,cursor,superConCallExpressions);
		++cursor; // step over ')'
//		std::cout << " #### ";
	}

	ctxt.blocks.push_back(NULL); // mark beginning of new local namespace
	Block * block=dynamic_cast<Block*>(getExpression(ctxt,cursor));
	if (block==NULL) {
		std::cout << tokens.at(cursor)->toString();
//
//		out(tokens.at(cursor));
		throw new Error("[fn] Expects Block of statements.",tokens.at(cursor));
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


/**
 * Reads a Control-Statement from tokens beginning at index "cursor".
 * Cursor is placed at the last Token of the statement.
 * @param tokens Program as Token-List.
 * @param curosr Cursor pointing at current Token.
 * @return Control-statement or NULL if no Control-Statement could be read.
 */
Statement Parser::getControl(ParsingContext & ctxt,int & cursor)const throw(Exception *) {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TControl * tc=Token::cast<TControl>(tokens.at(cursor));
	if (!tc) return Statement(Statement::TYPE_UNDEFINED);
	++cursor;

	identifierId cId=tc->getId();
	/// if-Control
	if(cId==Consts::IDENTIFIER_if){
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throw new Error("[if] expects (",tokens.at(cursor));
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throw new Error("[if] expects (...)",tokens.at(cursor));
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
							new IfControl(condition,
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
			throw new Error("[for] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Statement initExp=createStatement(getExpression(ctxt,cursor));
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			std::cout << tokens.at(cursor)->toString();
//			out(tokens.at(cursor));
			throw new Error("[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throw new Error("[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Statement incr=createStatement(getExpression(ctxt,cursor));
		if (incr.isValid())
			++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			std::cout << tokens.at(cursor)->toString();
//			out(tokens.at(cursor));
			throw new Error("[for] expects )",tokens.at(cursor));
		}
		++cursor;
		Statement action=getStatement(ctxt,cursor);

		loopWrappingBlock->addStatement( initExp );
		loopWrappingBlock->setJumpPosA( loopWrappingBlock->getNextPos() );
		if(condition!=NULL)
			loopWrappingBlock->addStatement( Statement(
						Statement::TYPE_IF,
						new IfControl(condition,action,Statement(Statement::TYPE_BREAK))));
		else if(action.isValid())
			loopWrappingBlock->addStatement(action);

		loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
		loopWrappingBlock->addStatement( incr );
		loopWrappingBlock->addStatement( Statement( Statement::TYPE_JUMP_TO_A ) );
		loopWrappingBlock->setBreakPos( Block::POS_HANDLE_AND_LEAVE );

		return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
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
			throw new Error("[while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throw new Error("[while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		Statement action=getStatement(ctxt,cursor);
		loopWrappingBlock->setContinuePos( Block::POS_HANDLE_AND_RESTART );
		loopWrappingBlock->setJumpPosA( Block::POS_HANDLE_AND_RESTART );
		loopWrappingBlock->addStatement( Statement(
						Statement::TYPE_IF,
						new IfControl(condition,action,Statement(Statement::TYPE_BREAK))));
		loopWrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A));

		loopWrappingBlock->setBreakPos( Block::POS_HANDLE_AND_LEAVE );

		return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
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
			throw new Error("[do-while] expects while",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBlock>(tokens.at(cursor))) // do{} while{...};
			throw new Error("[do-while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=getExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throw new Error("[do-while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throw new Error("[do-while] expects ;",tokens.at(cursor));
		}

		loopWrappingBlock->setJumpPosA( Block::POS_HANDLE_AND_RESTART );
		loopWrappingBlock->addStatement( action );
		loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
		loopWrappingBlock->addStatement( Statement(
						Statement::TYPE_IF,
						new IfControl(condition, Statement(Statement::TYPE_JUMP_TO_A),Statement())));
		loopWrappingBlock->setBreakPos( Block::POS_HANDLE_AND_LEAVE );

		return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);

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
			throw new Error("[foreach] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		Block * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * arrayExpression=getExpression(ctxt,cursor);
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_as)
			throw new Error("[foreach] expects as",tokens.at(cursor));
		++cursor;

		TIdentifier * valueIdent=NULL;
		TIdentifier * keyIdent=NULL;
		if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
			throw new Error("[foreach] expects Identifier-1",tokens.at(cursor));
		++cursor;

		if (Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			keyIdent=valueIdent;
			if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
				throw new Error("[foreach] expects Identifier-2",tokens.at(cursor));
			++cursor;
		}

		if (!Token::isA<TEndBlock>(tokens.at(cursor)))
			throw new Error("[foreach] expects )",tokens.at(cursor));
		++cursor;
		Statement action=getStatement(ctxt,cursor);

		static const identifierId itId(stringToIdentifierId("__id"));
		static const identifierId getIteratorId(stringToIdentifierId("getIterator"));
		static const identifierId keyFnId(stringToIdentifierId("key"));
		static const identifierId valueFnId(stringToIdentifierId("value"));

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
					runtime.error("Could not get iterator.",parameter[0].get());
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
			Object * getIterator = new FunctionCall(new Function(fnWrapper::esf_getIterator), paramExp);
			loopWrappingBlock->addStatement( createStatement(getIterator));
		}

		loopWrappingBlock->setJumpPosA( loopWrappingBlock->getNextPos() );
		// if( __it.end() )	break;
		static const identifierId endId(stringToIdentifierId("end"));
		Object * condition = new FunctionCall(
								new GetAttribute(new GetAttribute(NULL,itId),endId ),std::vector<ObjRef>());
		loopWrappingBlock->addStatement( Statement(
						Statement::TYPE_IF,
						new IfControl(condition,Statement(Statement::TYPE_BREAK),Statement())));

		{ // key == __it.key(), value==__it.value()
			std::vector<ObjRef> paramExp;
			paramExp.push_back( Identifier::create(valueIdent->getId()));
			if(keyIdent!=NULL)
				paramExp.push_back( Identifier::create(keyIdent->getId()));
			loopWrappingBlock->addStatement( createStatement(new FunctionCall(new Function(fnWrapper::esf_setValues), paramExp)));

		}
		// [action]
		loopWrappingBlock->addStatement( action );
		// :continue
		loopWrappingBlock->setContinuePos( loopWrappingBlock->getNextPos() );
		// __it.next();
		static const identifierId nextFnId(stringToIdentifierId("next"));
		loopWrappingBlock->addStatement( createStatement(
						new FunctionCall(
							new GetAttribute(new GetAttribute(NULL,itId),nextFnId ),std::vector<ObjRef>())));
		// goto :second
		loopWrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A) );

		loopWrappingBlock->setBreakPos( Block::POS_HANDLE_AND_LEAVE );

		return Statement(Statement::TYPE_BLOCK,loopWrappingBlock);
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
		Object * tryBlock=getExpression(ctxt,cursor); // TODO should be a block
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_catch)
			throw new Error("[try-catch] expects catch",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throw new Error("[try-catch] expects (",tokens.at(cursor));
		++cursor;
		TIdentifier * tIdent=NULL;

		identifierId varName=0;
		bool hasVarName=false;
		if ((tIdent=Token::cast<TIdentifier>(tokens.at(cursor)))) {
			++cursor;
			varName=tIdent->getId();
			hasVarName=true;
		}

		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throw new Error("[try-catch] expects ([Identifier])",tokens.at(cursor));
		}
		++cursor;

		TStartBlock * tStartCatchBlock = Token::cast<TStartBlock>(tokens.at(cursor));
		if(tStartCatchBlock==NULL){
			throw new Error("[catch] expects Block {...}",tokens.at(cursor));
		}

		Block * catchBlock=tStartCatchBlock->getBlock();
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
					new SetAttribute(NULL,varName,
						new FunctionCall( new Function(fnWrapper::extractExceptionValue),
							paramExp,false,currentFilename,tStartCatchBlock->getLine()),SetAttribute::ASSIGN)));

		}
		getExpression(ctxt,cursor); // fill rest of catch block

		Block * wrappingBlock = new Block();
		wrappingBlock->addStatement( createStatement(tryBlock) );
		wrappingBlock->addStatement( Statement(Statement::TYPE_JUMP_TO_A) );
		wrappingBlock->setExceptionPos( wrappingBlock->getNextPos() );
		wrappingBlock->addStatement( Statement(Statement::TYPE_BLOCK,catchBlock) );
		wrappingBlock->setJumpPosA(  Block::POS_HANDLE_AND_LEAVE );
		return Statement(Statement::TYPE_BLOCK,wrappingBlock);
//		if (hasVarName)
//			dynamic_cast<Block *>(catchBlock)->declareVar(varName);
//		return createStatement(new TryCatchControl(tryBlock,dynamic_cast<Block *>(catchBlock),varName));
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
		throw new Error(string("Parsing Unimplemented Control:")+tc->toString(),tokens.at(cursor));
	}
}

/*!	getLValue
	\todo change string -> identifierId
*/
Parser::lValue_t Parser::getLValue(ParsingContext & ctxt,int from,int to,Object * & obj,
								identifierId & identifier,Object * &indexExpression)const throw(Exception *) {
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
			throw new Error("LValue Error 1",tokens[from]);
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
				identifier=stringToIdentifierId(s->toString());
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

				std::cout << "\n!";//<<tokens.at(cursor)->toString();
				for(int i=cursor;i<to;++i)
					std::cout << " "<<tokens[i]->toString();
				throw new Error("Unexpected Ending.",tokens.at(cursor));
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
				throw new Error("Expressions can't contain control statements.",t);
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
UserFunction::parameterList_t * Parser::getFunctionParameters(ParsingContext & ctxt,int & cursor)const throw(Exception *) {

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
		identifierId name=0;
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
						throw new Error("[fn] Only the last parameter may be a multiparameter.",tokens[c]);
					break;
				}else if(  Token::isA<TOperator>(tNext) && tNext->toString()=="=" ){
					int defaultExpStart=c+2;
					int defaultExpTo=findExpression(ctxt,defaultExpStart);
					defaultExpression=getExpression(ctxt,defaultExpStart,defaultExpTo);
					if (defaultExpression==NULL) {
						throw new Error("[fn] SyntaxError in default parameter.",tokens.at(cursor));
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
				throw new Error("[fn] Error in parameter definition.",t);
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
		throw new Error("[fn] SyntaxError.",tokens[c+1]);
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

/*!	1,bla+2,(3*3)
	Cursor is moved at closing bracket ')'
*/
void Parser::getExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const throw (Exception *){
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();
	if(t->toString()!="(") {
		std::cout << " #"<<t->toString();
		throw new Error("Expression list error.",t);
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
			throw new Error("Expected )",tokens.at(cursor));
		}
	}
}

}
