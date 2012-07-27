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

//!	(ctor)
Parser::Parser(Logger * _logger) :
		logger(_logger ? _logger : new StdLogger(std::cout)) {
	//ctor
}
Parser::~Parser(){}


void Parser::log(ParsingContext & ctxt,Logger::level_t messageLevel, const std::string & msg,const _CountedRef<Token> & token)const{
	std::ostringstream os;
	os << "[Parser] " << msg << " (" << ctxt.code.getFilename();
	if(token!=NULL)
		os << ':' << token->getLine();
	os << ").";
	logger->log(messageLevel,os.str());
}

ERef<AST::BlockExpr> Parser::parse(const CodeFragment & code) {
	ERef<AST::BlockExpr> rootBlock = new AST::BlockExpr;

	tokenizer.defineToken("__FILE__",new TObject(String::create(code.getFilename())));
	tokenizer.defineToken("__DIR__",new TObject(String::create(IO::dirname(code.getFilename()))));

	Tokenizer::tokenList_t tokens;
	ParsingContext ctxt(tokens,code);
	ctxt.rootBlock = rootBlock.get();

	/// 1. Tokenize
	try {
		tokenizer.getTokens(code.getCodeString().c_str(),tokens); //! \todo Use codeFragment for Tokenizer
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

	/// 3. Parse expressions
	int cursor=0;
	readStatement(ctxt,cursor);

	return rootBlock;
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
 * - disambiguate Map/BlockExpr
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
					throwError(ctxt,"Syntax Error: ')'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndIndex::TYPE_ID:{
				if (!Token::isA<TStartIndex>(cbi.token)) {
					throwError(ctxt,"Syntax Error: ']'",token);
				}
				bInfStack.pop();
				continue;
			}
			case TEndBlock::TYPE_ID:{
				if (!cbi.isCommandBlock) {
					throwError(ctxt,"Syntax Error: '}'",token);
				}
				/// BlockExpr is Map Constructor
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
					throwError(ctxt,"Syntax Error in BlockExpr: ':'",token);
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
					throwError(ctxt,"Syntax Error in Map: ';'",token);
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
		throwError(ctxt,"Unexpected eof (unclosed '"+bInfStack.top().token->toString()+"'?)",bInfStack.top().token);
	}
}

/**
 * Pass 2
 * =========
 * - Create BlockExpr-Objects
 * - Parse declarations (var)
 * - Wrap parts of "fn" in brackets for easier processing: fn(foo,bar){...}  --->  fn( (foo,bar){} )
 * - Change loop brackets to blocks to handle loop wide scope handling: while(...) ---> while{...}
 * - TODO: Class declaration
 * ?????- TODO: Undefined scope for i: "{ var i;  do{ var i; }while(var i); }"
 */
void Parser::pass_2(ParsingContext & ctxt,
					Tokenizer::tokenList_t & enrichedTokens)const  {

	std::stack<BlockExpr *> blockStack;
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
							log(ctxt,Logger::LOG_WARNING, "Duplicate local variable '"+ti->toString()+'\'',ti);
						}
//						Token::removeReference(token);
						continue;
					} else
						throwError(ctxt,"var expects Identifier.",tc);
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
					BlockExpr * loopConditionBlock=new BlockExpr(tc->getLine());
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
			/// Open new BlockExpr
			case TStartBlock::TYPE_ID:{
				TStartBlock * sb=Token::cast<TStartBlock>(token);
				BlockExpr * currentBlock=new BlockExpr(sb->getLine());//currentBlock);

				blockStack.push(currentBlock);
				sb->setBlock(currentBlock);
				enrichedTokens.push_back(token);

				if(!functionBracketDepth.empty())
					++functionBracketDepth.top();

				continue;
			}
			/// Close BlockExpr
			case TEndBlock::TYPE_ID:{
				enrichedTokens.push_back(token);

				blockStack.pop();
				if (blockStack.empty())
					throwError(ctxt,"Unexpected }");

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
					throwError(ctxt,"Missing opening bracket for ",token);

				// add shortcut to the closing bracket
				currentBracket.top()->endBracketIndex=enrichedTokens.size()-1;
				currentBracket.pop();
				continue;
			}
			// "part1" "part2"
			case TObject::TYPE_ID:{
				enrichedTokens.push_back(token);
		
				// no consecutive constants OR first one is not a string?
				if( ctxt.tokens.at(cursor+1)->getType()!=TObject::TYPE_ID || 
						Token::cast<TObject>(token)->obj->_getInternalTypeId()!=_TypeIds::TYPE_STRING ){
					continue;
				}
				std::stringstream os;
				String * firstString = Token::cast<TObject>(token)->obj.toType<String>();
	
				os << token->toString();
				for(TObject * next=Token::cast<TObject>(ctxt.tokens.at(cursor+1)); 
						next!=NULL && next->obj->_getInternalTypeId()==_TypeIds::TYPE_STRING;
						++cursor, next = Token::cast<TObject>(ctxt.tokens.at(cursor+1))){
					os << next->toString();	
				}
				firstString->setString(os.str());
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
					throwError(ctxt,"Unclosed {");

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
 * Object * readExpression(...)
 *
 *
 * Cursor is moved to the last position of the Expression.
 *
 */
Object * Parser::readExpression(ParsingContext & ctxt,int & cursor,int to)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (cursor>=static_cast<int>(tokens.size())){
		return NULL;
	}/// Commands: if(...){}
	else if (Token::isA<TControl>(tokens.at(cursor))) {
		log(ctxt,Logger::LOG_WARNING, "No control statement here!",tokens.at(cursor));
		return NULL;
	} /// BlockExpr: {...}
	else if (Token::isA<TStartBlock>(tokens.at(cursor))) {
		return readBlock(ctxt,cursor);
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
				BlockExpr * b=ctxt.blocks.at(i);
				if(b==NULL)
					break;
				 else if(b->isLocalVar(ident->getId())){
//////					std::cout <<"local:"<<ident->toString()<<"\n";
					break;
				 }
		}
			return new GetAttributeExpr(NULL,ident->getId());  // ID
		}
		throwError(ctxt,"Unknown (or unimplemented) Token",t);
	}

	///  Command ends with ;
	///  "2;"
	/// ---------------------
	else if (Token::isA<TEndCommand>(tokens[to])) {
		Object * e=readExpression(ctxt,cursor,to-1);
		cursor = to;
		return e;
	}

	/// Surrounded with Brackets
	/// "(a+2)"
	/// --------------------------
	else if (Token::isA<TStartBracket>(tokens.at(cursor)) &&
			 Token::isA<TEndBracket>(tokens[to]) &&
			 findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to,1)==to) {

		++cursor;
		Object * innerExpression=readExpression(ctxt,cursor,to-1);
		cursor = to;
		return innerExpression;
	}

	/// Map Constructor
	/// "{foo:bar,2:3}"
	/// --------------------------
	if (Token::isA<TStartMap>(tokens.at(cursor)) &&
		Token::isA<TEndMap>(tokens[to]) &&
		findCorrespondingBracket<TStartMap,TEndMap>(ctxt,cursor,to,1) == to) {
		return readMap(ctxt,cursor);
	}

	/// BinaryExpression
	/// "3+foo"
	/// --------------------------
	if (Object * obj=readBinaryExpression(ctxt,cursor,to)) {
		return obj;
	}

	///    Syntax Error
	/// --------------------
	else {
		throwError(ctxt,"Syntax error",tokens.at(cursor).get());
		return NULL;
	}
}


//! (internal)
Statement Parser::readStatement(ParsingContext & ctxt,int & cursor,int to)const{

	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (Token::isA<TControl>(tokens.at(cursor))) {
		return readControl(ctxt,cursor);
	} /// sub-BlockExpr: {...}
	else if (Token::isA<TStartBlock>(tokens.at(cursor))) {
		return Statement(Statement::TYPE_STATEMENT, readBlock(ctxt,cursor));
	}else{
		Object * exp=readExpression(ctxt,cursor,to);
		if(exp)
			return Statement(Statement::TYPE_EXPRESSION,exp);
		return Statement(Statement::TYPE_UNDEFINED);
	}

}

/**
 * Get block of statements
 * {out("foo");exit;}
 */
BlockExpr * Parser::readBlock(ParsingContext & ctxt,int & cursor)const {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TStartBlock * tsb=Token::cast<TStartBlock>(tokens.at(cursor));
	BlockExpr * b=tsb?reinterpret_cast<BlockExpr *>(tsb->getBlock()):NULL;
	if (b==NULL)
		throwError(ctxt,"No BlockExpr!",tokens.at(cursor));

	/// Check for shadowed local variables
	if(!ctxt.blocks.empty()){
		const BlockExpr::declaredVariableMap_t & vars = b->getVars();
		if(!vars.empty()){
			for(int i = ctxt.blocks.size()-1; i>=0 && ctxt.blocks[i]!=NULL; --i ){
				const BlockExpr::declaredVariableMap_t & vars2 = ctxt.blocks[i]->getVars();
				if(vars2.empty())
					continue;
				for(BlockExpr::declaredVariableMap_t::const_iterator it=vars.begin();it!=vars.end();++it){
					if(vars2.count(*it)>0){
						log(ctxt,Logger::LOG_PEDANTIC_WARNING,"Shadowed local variable  '"+(*it).toString()+"' in block.",tokens.at(cursor));
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
			throwError(ctxt,"Unclosed BlockExpr {...",tsb);

		int line=tokens.at(cursor)->getLine();
		Statement stmt=readStatement(ctxt,cursor);

		if(stmt.isValid()){
			stmt.setLine(line);
			b->addStatement(stmt);
		}

		/// Commands have to end on ";" or "}".
		if (!(Token::isA<TEndCommand>(tokens.at(cursor)) || Token::isA<TEndBlock>(tokens.at(cursor)))) {
			log(ctxt,Logger::LOG_DEBUG, tokens.at(cursor)->toString(),tokens.at(cursor));
			throwError(ctxt,"Syntax Error in BlockExpr.",tokens.at(cursor));
		}
		++cursor;
	}
	ctxt.blocks.pop_back();
	return b;
}

//!	readMap
Object * Parser::readMap(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	if (!Token::isA<TStartMap>(tokens.at(cursor)))
		throwError(ctxt,"No Map!",tokens.at(cursor));

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
			exp=readExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		/// ii) read ":"
		if (!Token::isA<TMapDelimiter>(tokens.at(cursor))) {
			log(ctxt,Logger::LOG_DEBUG, tokens.at(cursor)->toString(),tokens.at(cursor));
			throwError(ctxt,"Map: Expected : ",tokens.at(cursor));
		}
		++cursor;

		/// iii) read Value
		/// Value is not present (only valid for last tuple)
		if (Token::isA<TEndMap>(tokens.at(cursor))) {
			exp=Void::get();
		} /// Value is present
		else {
			exp=readExpression(ctxt,cursor);
			++cursor;
		}
		paramExp.push_back(exp);

		if (Token::isA<TEndMap>(tokens.at(cursor)))
			break;
		else if (Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			continue;
		} else
			throwError(ctxt,"Map Syntax Error",tokens.at(cursor));
	}

	return FunctionCallExpr::createSysCall(Consts::SYS_CALL_CREATE_MAP,paramExp,currentLine);

}

/*!	Binary expression	*/
Object * Parser::readBinaryExpression(ParsingContext & ctxt,int & cursor,int to)const  {
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
				throwError(ctxt,"Error in binary expression",t);
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

		Object * rightExpression=readExpression(ctxt,rightExprFrom,to);
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
			return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(obj,Consts::IDENTIFIER_fn_set),paramExp,currentLine);
		} else {
//			std::cout << "\n Error = "<<cursor<<" - "<<to<<" :" << lValueType;
			throwError(ctxt,"No valid LValue before '=' ",tokens[opPosition]);
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
					log(ctxt,Logger::LOG_INFO,"Property:"+name.toString(),atOp );
//					const int pos = it->second;
					if(name == Consts::PROPERTY_ATTR_const){
						flags |= Attribute::CONST_BIT;
					}else if(name == Consts::PROPERTY_ATTR_init){
						if(flags&Attribute::TYPE_ATTR_BIT)
							log(ctxt,Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
						flags |= Attribute::INIT_BIT;
					}else if(name == Consts::PROPERTY_ATTR_member){
						if(flags&Attribute::TYPE_ATTR_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::TYPE_ATTR_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_override){
						flags |= Attribute::OVERRIDE_BIT;
					}else if(name == Consts::PROPERTY_ATTR_private){
						if(inverseFlags&Attribute::PRIVATE_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(private)' is used in combination with @(public) and is ignored.",atOp);
						}else{
							flags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_public){
						if(flags&Attribute::PRIVATE_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(public)' is used in combination with @(private) and is ignored.",atOp);
						}else{
							inverseFlags |= Attribute::PRIVATE_BIT;
						}
					}else if(name == Consts::PROPERTY_ATTR_type){
						if(inverseFlags&Attribute::TYPE_ATTR_BIT){
							log(ctxt,Logger::LOG_WARNING,"'@(member)' is used in combination with @(type) or '::=' and is ignored.",atOp);
						}else{
							flags |= Attribute::TYPE_ATTR_BIT;
						}
						if(flags&Attribute::INIT_BIT)
							log(ctxt,Logger::LOG_WARNING,"'@(init)' is used in combination with @(type) or '::='.",atOp);
					}else {
						throwError(ctxt,"Invalid property: '"+name.toString()+'\'',atOp);
					}
				}
			}

		}
		StringId memberIdentifier;
		Object * obj=NULL;

		Object * indexExp=NULL;
		const int lValueType=getLValue(ctxt,leftExprFrom,leftExprTo,obj,memberIdentifier,indexExp);

		Object * rightExpression=readExpression(ctxt,rightExprFrom,to);
		cursor=rightExprFrom;


		/// a:=2 => _.[a] := 2
		if (lValueType != LVALUE_MEMBER) {
			throwError(ctxt,"No valid member-LValue before '"+op->getString()+"' ",tokens[opPosition]);
		}
		if(obj==NULL){
			log(ctxt,Logger::LOG_WARNING,"Use '=' for assigning to local variables instead of '"+op->getString()+"' ",tokens[opPosition]);
			return SetAttributeExpr::createAssignment(obj,memberIdentifier,rightExpression,currentLine);
		}
		return new SetAttributeExpr(obj,memberIdentifier,rightExpression,flags,currentLine);
	}


	/// get left expression
	Object * leftExpression=readExpression(ctxt,leftExprFrom,leftExprTo);

	/// "a.b.c"
	if (op->getString()==".") {
		if (rightExprFrom>to) {
			log(ctxt,Logger::LOG_DEBUG, "Error .1 ",tokens[opPosition]);
			throwError(ctxt,"Syntax error after '.'",tokens[opPosition]);
		}
		cursor = to;

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
		log(ctxt,Logger::LOG_DEBUG, "Error .2 ",tokens[opPosition]);
		throwError(ctxt,"Syntax error after '.'",tokens[opPosition]);
	}
	///  Function Call
	/// "a(b)"  "a(1,2,3)"
	else if (op->getString()=="(") {
		cursor=rightExprFrom-1;
		std::vector<ObjRef> paramExp;
		readExpressionsInBrackets(ctxt,cursor,paramExp);

		if(cursor!=to){
			throwError(ctxt,"Error after function call. Forgotten ';' ?",tokens.at(cursor));
		}
		FunctionCallExpr * funcCall = FunctionCallExpr::createFunctionCall(leftExpression,paramExp,currentLine);
		return funcCall;
	}
	///  Index Exression | Array
	else if (op->getString()=="[") {
		/// No left expression present? -> Array-constructor
		///"[1,a+2,3]" -> new Array(1,a+2,3)
		if (!leftExpression) {
			std::vector<ObjRef> paramExps;
			++cursor;
			while (!Token::isA<TEndIndex>(tokens.at(cursor)) ) {

				paramExps.push_back(readExpression(ctxt,cursor));

				++cursor;
				if (Token::isA<TDelimiter>(tokens.at(cursor)))
					++cursor;
				else if (!Token::isA<TEndIndex>(tokens.at(cursor))){
					throwError(ctxt,"Expected ]",tokens[opPosition]);
				}
			}
			cursor = to;
			return FunctionCallExpr::createSysCall( Consts::SYS_CALL_CREATE_ARRAY,
												paramExps,currentLine);
		}
		/// Left expression present? -> Index Expression
		/// "a[1]"
		cursor=rightExprFrom;
		std::vector<ObjRef> paramExps;
		paramExps.push_back(readExpression(ctxt,cursor));
		cursor = to;
		return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(leftExpression,Consts::IDENTIFIER_fn_get),
										paramExps,currentLine);

	}/// "a?1:2"
	else if (op->getString()=="?") {
		cursor=rightExprFrom;
		Object * alt1=readExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TColon>(tokens.at(cursor))) {
			throwError(ctxt,"Expected ':'",tokens.at(cursor));
		}
		++cursor;
		Object * alt2=readExpression(ctxt,cursor);
		return new ConditionalExpr(leftExpression,alt1,alt2);
	} /// new Object
	else if (op->getString()=="new") {
		++cursor;
		if (leftExpression)
			throwError(ctxt,"new is a unary left operator.",tokens.at(cursor));

		int objExprTo=to;

		/// if new has paramteres "(...)", search for their beginning.
		if (Token::isA<TEndBracket>(tokens[objExprTo])) {
			objExprTo=findCorrespondingBracket<TEndBracket,TStartBracket>(ctxt,objExprTo,rightExprFrom,-1);
		}
		/// read parameters
		std::vector<ObjRef> paramExp;
		if (objExprTo>cursor) {
			int cursor2=objExprTo;
			readExpressionsInBrackets(ctxt,cursor2,paramExp);

			objExprTo--; /// why ?????????????
		}
		/// read Object-expression
		Object * obj=readExpression(ctxt,cursor,objExprTo);
		cursor = to;

		return FunctionCallExpr::createConstructorCall(obj,paramExp,currentLine);
	}
	/// Function "fn(a,b){return a+b;}"
	else if (op->getString()=="fn" ){//|| op->getString()=="lambda") {
		ObjRef result=readFunctionDeclaration(ctxt,cursor);
		if (cursor!=to)    {
			throwError(ctxt,"[fn] Syntax error.",tokens.at(cursor));
		}
		return result.detachAndDecrease();
	}

	Object * rightExpression=readExpression(ctxt,rightExprFrom,to);

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
			return LogicOpExpr::createNot(rightExpression) ;
		}

		//if (GetAttributeExpr * ga=dynamic_cast<GetAttributeExpr *>(rightExpression)) {
		FunctionCallExpr * fc = FunctionCallExpr::createFunctionCall(
			new GetAttributeExpr(rightExpression,
							 string(op->getString())+"_pre"),std::vector<ObjRef>(),currentLine);
		return  fc;

	} else
		/// Unary postfix expression
		/// a++, a--, a!
		/// Bsp: a++ => _.a.++post()
		if (!rightExpression) {
			//  if (GetAttributeExpr * ga=dynamic_cast<GetAttributeExpr *>(leftExpression)) {
			FunctionCallExpr * fc=FunctionCallExpr::createFunctionCall(
				new GetAttributeExpr(leftExpression,
								 string(op->getString())+"_post"),std::vector<ObjRef>(),currentLine);
			cursor--;

			return  fc;
		}
	/// ||
		else if (op->getString()=="||") {
			return LogicOpExpr::createOr(leftExpression,rightExpression);
		}
	/// &&
		else if (op->getString()=="&&") {
			return LogicOpExpr::createAnd(leftExpression,rightExpression);
		}
	/// normal binary expression
	/// 1+2 -> 1.+(2)
		else {
			std::vector<ObjRef> paramExp;
			paramExp.push_back(rightExpression);
			return FunctionCallExpr::createFunctionCall(new GetAttributeExpr(leftExpression, op->getString()),
													paramExp,currentLine);
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
Object * Parser::readFunctionDeclaration(ParsingContext & ctxt,int & cursor)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();

	if(t->toString()!="fn"){
		throwError(ctxt,"No function! ",tokens.at(cursor));
	}
	const size_t codeStartPos = t->getStartingPos();
	const size_t line = t->getLine();

	++cursor;

	/// step over '(' inserted at pass_2(...)
	++cursor;

	UserFunctionExpr::parameterList_t params;
	readFunctionParameters(params,ctxt,cursor);
	TOperator * superOp=Token::cast<TOperator>(tokens.at(cursor));

	/// fn(a).(a+1,2){} \deprecated
	std::vector<ObjRef> superConCallExpressions;
	if(superOp!=NULL && superOp->toString()=="."){
		++cursor;
		readExpressionsInBrackets(ctxt,cursor,superConCallExpressions);
		++cursor; // step over ')'

	} /// fn(a)@(super(a+1,2)) {}
	else if(superOp!=NULL && superOp->toString()=="@"){
		++cursor;
		if(!Token::isA<TStartBracket>(tokens.at(cursor))){
			throwError(ctxt,"Property expects brackets.",superOp);
		}
		const int propertyTo = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor);

		properties_t properties;
		readProperties(ctxt,cursor+1,propertyTo-1,properties);
		for(properties_t::const_iterator it=properties.begin();it!=properties.end();++it ){
			const StringId name = it->first;
			int parameterPos = it->second;
			log(ctxt,Logger::LOG_INFO,"Property:"+name.toString(),superOp );
//					const int pos = it->second;
			if(name == Consts::PROPERTY_FN_super){
				if(parameterPos<0){
					throwError(ctxt,"Super attribute needs parameter list.",superOp);
				}
				readExpressionsInBrackets(ctxt,parameterPos,superConCallExpressions);
			}else{
				log(ctxt,Logger::LOG_WARNING,"Anntoation is invalid for functions: '"+name.toString()+"'",superOp);
			}
		}
		cursor = propertyTo+1;

	}


	ctxt.blocks.push_back(NULL); // mark beginning of new local namespace
	BlockExpr * block=dynamic_cast<BlockExpr*>(readExpression(ctxt,cursor));
	if (block==NULL) {
		throwError(ctxt,"[fn] Expects BlockExpr of statements.",tokens.at(cursor));
	}
	ctxt.blocks.pop_back(); // remove marking for local namespace

	const size_t codeEndPos = tokens.at(cursor)->getStartingPos(); // position of '}'

	/// step over ')' inserted at pass_2(...)
	++cursor;

	{	// create function expression
		UserFunctionExpr * uFunExpr = new UserFunctionExpr(block,superConCallExpressions,line);
		uFunExpr->getParamList().swap(params);	// set parameter expressions

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
 * @param curosr Cursor pointing at current Token.
 * @return Control-statement or NULL if no Control-Statement could be read.
 */
Statement Parser::readControl(ParsingContext & ctxt,int & cursor)const  {
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	TControl * tc=Token::cast<TControl>(tokens.at(cursor));
	if (!tc) return Statement(Statement::TYPE_UNDEFINED);
	++cursor;

	StringId cId=tc->getId();
	/// if-Control
	if(cId==Consts::IDENTIFIER_if){
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError(ctxt,"[if] expects (",tokens.at(cursor));
		++cursor;
		Object * condition=readExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"[if] expects (...)",tokens.at(cursor));
		}
		++cursor;
		Statement action=readStatement(ctxt,cursor);
		Statement elseAction;
		if ((tc=Token::cast<TControl>(tokens.at(cursor+1)))) {
			if (tc->getId()==Consts::IDENTIFIER_else) {
				++cursor;
				++cursor;
				elseAction=readStatement(ctxt,cursor);
			}
		}
		return Statement(Statement::TYPE_STATEMENT,
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
			throwError(ctxt,"[for] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockExpr * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Statement initExp = readStatement(ctxt,cursor);
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Object * condition=readExpression(ctxt,cursor);
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects ;",tokens.at(cursor));
		}
		++cursor;
		Statement incr = readStatement(ctxt,cursor);
		if (incr.isValid())
			++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[for] expects )",tokens.at(cursor));
		}
		++cursor;
		Statement action=readStatement(ctxt,cursor);


		loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT,
													LoopStatement::createForLoop(initExp,condition,incr,action) ) );
		return Statement(Statement::TYPE_STATEMENT,loopWrappingBlock);
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
			throwError(ctxt,"[while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockExpr * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=readExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		Statement action=readStatement(ctxt,cursor);

		loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, LoopStatement::createWhileLoop(condition,action) ) );
		return Statement(Statement::TYPE_STATEMENT,loopWrappingBlock);
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
		Statement action=readStatement(ctxt,cursor);
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_while)
			throwError(ctxt,"[do-while] expects while",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBlock>(tokens.at(cursor))) // do{} while{...};
			throwError(ctxt,"[do-while] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockExpr * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * condition=readExpression(ctxt,cursor);
		++cursor;
		if (!Token::isA<TEndBlock>(tokens.at(cursor))) {
			throwError(ctxt,"[do-while] expects (...)",tokens.at(cursor));
		}
		++cursor;
		if (!Token::isA<TEndCommand>(tokens.at(cursor))) {
			throwError(ctxt,"[do-while] expects ;",tokens.at(cursor));
		}

		loopWrappingBlock->addStatement( Statement(Statement::TYPE_STATEMENT, LoopStatement::createDoWhileLoop(condition,action) ) );
		return Statement(Statement::TYPE_STATEMENT,loopWrappingBlock);
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
			for( __it=obj.getIterator(); !__it.end() ; __it.next() ){
				key = __it.key();
				value = __it.key();
			}
		}
	*/

	else if(cId==Consts::IDENTIFIER_foreach) {
		if (!Token::isA<TStartBlock>(tokens.at(cursor)))  // foreach{...as...}
			throwError(ctxt,"[foreach] expects (",tokens.at(cursor));
		// this block stores the running variables, defined in the loop condition
		BlockExpr * loopWrappingBlock = Token::cast<TStartBlock>(tokens.at(cursor))->getBlock();
		++cursor;
		Object * arrayExpression=readExpression(ctxt,cursor);
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_as)
			throwError(ctxt,"[foreach] expects as",tokens.at(cursor));
		++cursor;

		TIdentifier * valueIdent=NULL;
		TIdentifier * keyIdent=NULL;
		if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
			throwError(ctxt,"[foreach] expects Identifier-1",tokens.at(cursor));
		++cursor;

		if (Token::isA<TDelimiter>(tokens.at(cursor))) {
			++cursor;
			keyIdent=valueIdent;
			if (!(valueIdent=Token::cast<TIdentifier>(tokens.at(cursor))))
				throwError(ctxt,"[foreach] expects Identifier-2",tokens.at(cursor));
			++cursor;
		}

		if (!Token::isA<TEndBlock>(tokens.at(cursor)))
			throwError(ctxt,"[foreach] expects )",tokens.at(cursor));
		++cursor;
		Statement action=readStatement(ctxt,cursor);

		static const StringId itId("__it");

		// var __it;
		loopWrappingBlock->declareVar(itId);

		/* \todo speedup by using systemCall:
			for(__it = sysCall getIterator(arr); sysCall isIteratorEnd(__it); sysCall increasIterator (__it) )
		*/
		// __it = SYS_CALL_GET_ITERATOR( obj ) ( ~ __it = obj.getIterator();  + some special cases)
		std::vector<ObjRef> loopInitParams;
		loopInitParams.push_back(arrayExpression);
		Statement loopInit = Statement(Statement::TYPE_EXPRESSION,
			SetAttributeExpr::createAssignment(NULL,itId,
				FunctionCallExpr::createSysCall(Consts::SYS_CALL_GET_ITERATOR,loopInitParams,tokens.at(cursor)->getLine())));

		// ! __it.end()
		Object * checkExpression = LogicOpExpr::createNot(
				FunctionCallExpr::createFunctionCall(
					new GetAttributeExpr(new GetAttributeExpr(NULL,itId), Consts::IDENTIFIER_fn_it_end),std::vector<ObjRef>() ));

		// __it.next()
		Statement increaseStatement = Statement(Statement::TYPE_EXPRESSION,
				FunctionCallExpr::createFunctionCall(
					new GetAttributeExpr(new GetAttributeExpr(NULL,itId), Consts::IDENTIFIER_fn_it_next),std::vector<ObjRef>() ));

		BlockExpr * actionWrapper = new BlockExpr();

		// key = __it.key();
		if(keyIdent){
			actionWrapper->addStatement(Statement(Statement::TYPE_EXPRESSION,
				SetAttributeExpr::createAssignment(NULL,keyIdent->getId(),
					FunctionCallExpr::createFunctionCall(
						new GetAttributeExpr(
							new GetAttributeExpr(NULL,itId), Consts::IDENTIFIER_fn_it_key),std::vector<ObjRef>() ),tokens.at(cursor)->getLine())));
		}

		// value = __it.value();
		if(valueIdent){
			actionWrapper->addStatement(Statement(Statement::TYPE_EXPRESSION,
				SetAttributeExpr::createAssignment(NULL,valueIdent->getId(),
					FunctionCallExpr::createFunctionCall(
						new GetAttributeExpr(
							new GetAttributeExpr(NULL,itId), Consts::IDENTIFIER_fn_it_value),std::vector<ObjRef>() ),tokens.at(cursor)->getLine())));
		}
		actionWrapper->addStatement(action);

		loopWrappingBlock->addStatement(Statement(Statement::TYPE_STATEMENT,
				LoopStatement::createForLoop(loopInit,checkExpression,increaseStatement,
												Statement(Statement::TYPE_STATEMENT,actionWrapper))));

		return Statement(Statement::TYPE_STATEMENT,loopWrappingBlock);

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
		BlockExpr * tryBlock=dynamic_cast<BlockExpr*>(readExpression(ctxt,cursor));
		if(!tryBlock){
			throwError(ctxt,"[try-catch] expects an try block. ",tokens.at(cursor));
		}
		++cursor;
		tc=Token::cast<TControl>(tokens.at(cursor));
		if (!tc || tc->getId()!=Consts::IDENTIFIER_catch)
			throwError(ctxt,"[try-catch] expects catch",tokens.at(cursor));
		++cursor;
		if (!Token::isA<TStartBracket>(tokens.at(cursor)))
			throwError(ctxt,"[try-catch] expects (",tokens.at(cursor));
		++cursor;
		TIdentifier * tIdent=NULL;

		StringId varName;
		if ((tIdent=Token::cast<TIdentifier>(tokens.at(cursor)))) {
			++cursor;
			varName=tIdent->getId();
		}

		if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"[try-catch] expects ([Identifier])",tokens.at(cursor));
		}
		++cursor;

		TStartBlock * tStartCatchBlock = Token::cast<TStartBlock>(tokens.at(cursor));
		if(tStartCatchBlock==NULL){
			throwError(ctxt,"[catch] expects BlockExpr {...}",tokens.at(cursor));
		}

		BlockExpr * catchBlock=tStartCatchBlock->getBlock();
		readExpression(ctxt,cursor); // fill rest of catch block
		return Statement(Statement::TYPE_STATEMENT,new TryCatchStatement(tryBlock,catchBlock,varName));
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
		return Statement(Statement::TYPE_RETURN,readExpression(ctxt,cursor));
	}
	/// exit-Control
	else if(cId==Consts::IDENTIFIER_exit) {
		return Statement(Statement::TYPE_EXIT,readExpression(ctxt,cursor));
	}
	/// throw-Control
	else if(cId==Consts::IDENTIFIER_throw) {
		return Statement(Statement::TYPE_THROW,readExpression(ctxt,cursor));
	}
	/// yield-Control
	else if(cId==Consts::IDENTIFIER_yield) {
		return Statement(Statement::TYPE_YIELD,readExpression(ctxt,cursor));
	}
	else{
		throwError(ctxt,string("Parsing Unimplemented Control:")+tc->toString(),tokens.at(cursor));
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
			throwError(ctxt,"LValue Error 1",tokens[from]);
		}
	}
	/// ".a"
	/// "a.b.c"
	if (Token::isA<TIdentifier>(tokens[to]) && Token::isA<TOperator>(tokens[to-1]) ) {
		if ( Token::cast<TOperator>(tokens.at(to-1))->getOperator()->getString()==".") {
			obj=readExpression(ctxt,from,to-2);
			identifier=Token::cast<TIdentifier>(tokens[to])->getId();
			return LVALUE_MEMBER;
		}
	}
	if(TObject * tObj=Token::cast<TObject>(tokens[to])){
		/// ".'a'"
		/// "a.b.'c'"
		if (String * s=dynamic_cast<String *>(tObj->obj.get())) {
			TOperator * top=Token::cast<TOperator>(tokens[to-1]);

			if (top && top->getOperator()->getString()==".") {
				obj=readExpression(ctxt,from,to-2);
				identifier = s->toString();
				return LVALUE_MEMBER;
			}
		}
		/// ".$a"
		/// "a.b.$c"
		if (Identifier * i=dynamic_cast<Identifier *>(tObj->obj.get())) {
			TOperator * top=Token::cast<TOperator>(tokens[to-1]);

			if (top && top->getOperator()->getString()==".") {
				obj=readExpression(ctxt,from,to-2);
				identifier=i->getId();
				return LVALUE_MEMBER;
			}
		}
	}
	/// Index "a[1]"
	if (Token::isA<TEndIndex>(tokens[to])) {

		int indexOpenPos=findCorrespondingBracket<TEndIndex,TStartIndex>(ctxt,to,from,-1);
		/// a[1]
		if (indexOpenPos>from) {
			obj=readExpression(ctxt,from,indexOpenPos-1);
			indexOpenPos++;
			indexExpression=readExpression(ctxt,indexOpenPos,to-1);
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

				throwError(ctxt,"Unexpected Ending.",tokens.at(cursor));
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
				throwError(ctxt,"Expressions cannot contain control statements.",t);
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
						throwError(ctxt,"[fn] Only the last parameter may be a multiparameter.",tokens[c]);
					break;
				}else if(  Token::isA<TOperator>(tNext) && tNext->toString()=="=" ){
					int defaultExpStart=c+2;
					int defaultExpTo=findExpression(ctxt,defaultExpStart);
					defaultExpression=readExpression(ctxt,defaultExpStart,defaultExpTo);
					if (defaultExpression==NULL) {
						throwError(ctxt,"[fn] SyntaxError in default parameter.",tokens.at(cursor));
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
				throwError(ctxt,"[fn] Error in parameter definition.",t);
			}
			++c;
		}

		// get the type-check-expressions
		std::vector<ObjRef> typeExpressions;
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
					if (Token::isA<TDelimiter>(tokens.at(c2))){
						++c2;
					}else if (!Token::isA<TEndIndex>(tokens.at(c2))) {
						throwError(ctxt,"Expected ]",tokens.at(c2));
					}
				}while(!Token::isA<TEndIndex>(tokens.at(c2)));

			} // single type criterium: fn( Bool a){...}
			else{
				typeExpressions.push_back(readExpression(ctxt,c2,idPos-1));
			}
		}

		// test if this is the last parameter
		bool lastParam=false;
		if(Token::isA<TEndBracket>(tokens[c+1])){
		lastParam=true;
		}else if( ! Token::isA<TDelimiter>(tokens[c+1])){
			throwError(ctxt,"[fn] SyntaxError.",tokens[c+1]);
		}

		// move cursor
		cursor=c+2;

		// create parameter
		params.push_back(UserFunctionExpr::Parameter(name,NULL,typeExpressions));
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
void Parser::readExpressionsInBrackets(ParsingContext & ctxt,int & cursor,std::vector<ObjRef> & expressions)const{
	const Tokenizer::tokenList_t & tokens = ctxt.tokens;
	Token * t=tokens.at(cursor).get();
	if(t->toString()!="(") {
		throwError(ctxt,"Expression list error.",t);
	}
	++cursor;

	while (!Token::isA<TEndBracket>(tokens.at(cursor))) {
		if(Token::isA<TDelimiter>(tokens.at(cursor))){ // empty expression (1,,2)
			expressions.push_back(NULL);
			++cursor;
			continue;
		}
		expressions.push_back(readExpression(ctxt,cursor));
		++cursor;
		if (Token::isA<TDelimiter>(tokens.at(cursor))){
			++cursor;
		}else if (!Token::isA<TEndBracket>(tokens.at(cursor))) {
			throwError(ctxt,"Expected )",tokens.at(cursor));
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
			throwError(ctxt,"Identifier expected in property",t);

		int optionPos = -1;
		++cursor;
		if( Token::isA<TStartBracket>(tokens.at(cursor)) && cursor < to ){ // property has options 'property(exp1,exp2)'
			optionPos = cursor;
			cursor = findCorrespondingBracket<TStartBracket,TEndBracket>(ctxt,cursor,to); // skip expressions in brackets
			if(cursor<0)
				throwError(ctxt,"Unclosed option list in property",tokens.at(cursor));
			++cursor; // skip ')'
		}
		if(cursor<=to && !Token::isA<TDelimiter>(tokens.at(cursor))){ // expect a delimiter or the end.
			throwError(ctxt,"Syntax error in property",tokens.at(cursor));
		}
		properties.push_back( std::make_pair(tid->getId(),optionPos) );
	}
}


void Parser::throwError(ParsingContext & ctxt,const std::string & msg,Token * token)const{
	ParserException * e = new ParserException(msg,token);
	e->setFilename(ctxt.code.getFilename());
	throw e;
}

}
