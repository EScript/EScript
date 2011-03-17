// Tokenizer.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Tokenizer.h"
#include "Operators.h"
#include "../Utils/StringUtils.h"
#include "../Consts.h"
#include "../Objects/Identifier.h"
#include <sstream>

namespace EScript {

//! (static)
Token * Tokenizer::identifyStaticToken(identifierId id){
	static tokenMap_t constants;
	// init
	if(constants.empty()){
		constants[Consts::IDENTIFIER_if]=new TControl(Consts::IDENTIFIER_if);
		constants[Consts::IDENTIFIER_else]=new TControl(Consts::IDENTIFIER_else);
		constants[Consts::IDENTIFIER_do]=new TControl(Consts::IDENTIFIER_do);

		constants[Consts::IDENTIFIER_while]=new TControl(Consts::IDENTIFIER_while);
		constants[Consts::IDENTIFIER_break]=new TControl(Consts::IDENTIFIER_break);
		constants[Consts::IDENTIFIER_var]=new TControl(Consts::IDENTIFIER_var);

		constants[Consts::IDENTIFIER_continue]=new TControl(Consts::IDENTIFIER_continue);
		constants[Consts::IDENTIFIER_return]=new TControl(Consts::IDENTIFIER_return);
		constants[Consts::IDENTIFIER_exit]=new TControl(Consts::IDENTIFIER_exit);
		constants[Consts::IDENTIFIER_foreach]=new TControl(Consts::IDENTIFIER_foreach);
		constants[Consts::IDENTIFIER_as]=new TControl(Consts::IDENTIFIER_as);
		constants[Consts::IDENTIFIER_for]=new TControl(Consts::IDENTIFIER_for);

		constants[Consts::IDENTIFIER_try]=new TControl(Consts::IDENTIFIER_try);
		constants[Consts::IDENTIFIER_catch]=new TControl(Consts::IDENTIFIER_catch);
		constants[Consts::IDENTIFIER_throw]=new TControl(Consts::IDENTIFIER_throw);
		constants[Consts::IDENTIFIER_yield]=new TControl(Consts::IDENTIFIER_yield);
		constants[Consts::IDENTIFIER_namespace]=new TControl(Consts::IDENTIFIER_namespace);


		constants[Consts::IDENTIFIER_true]=new TObject(Bool::create(true));
		constants[Consts::IDENTIFIER_false]=new TObject(Bool::create(false));
		constants[Consts::IDENTIFIER_void]=new TObject(Void::get());
		constants[Consts::IDENTIFIER_null]=new TObject(Void::get());

	}
	tokenMap_t::const_iterator it=constants.find(id);
	return it==constants.end() ? NULL : it->second.get();
}

//! (internal)
Token * Tokenizer::identifyToken(identifierId id)const{
	tokenMap_t::const_iterator it=customTokens.find(id);
	if(it!=customTokens.end())
		return it->second.get();
	return identifyStaticToken(id);
}

//! (internal)
void Tokenizer::defineToken(const std::string & name,Token * value){
	customTokens[stringToIdentifierId(name)]=value;
}

//! (ctor)
Tokenizer::Tokenizer() {
	//ctor
}

//! (dtor)
Tokenizer::~Tokenizer() {
	//dtor
}

void Tokenizer::getTokens( const char * prog,tokenList & tokens)  throw (Exception *) {
	int cursor=0;
	int line=1;

	Token * obj;
	do {
		obj=readNextToken(prog,cursor,line,tokens);
		if (obj!=NULL) {
			obj->setLine(line);

			tokens.push_back(obj);
			Token::addReference(obj);
		}
	} while (!dynamic_cast<TEndScript *>(obj));

}

/**
 *   Reads the next Token from prog beginning with position cursor and moves
 *   cursor to the next Token.
 */
Token * Tokenizer::readNextToken(const char * prog, int & cursor,int &line,tokenList & tokens)  throw (Exception *) {

	char c=prog[cursor];

	// Step over whitespace characters
	while ( isWhitechar(c) || c=='\0') {
		if (c=='\n') line++;
		if (c=='\0')
			return new TEndScript();
		cursor++;
		c=prog[cursor];
	}

	// Multiline Comment
	// Returns 0 if a comment is read.
	if (c=='/'&& prog[cursor+1]=='*') {
		cursor+=2;
		if (prog[cursor]=='\0')
			throw(new Error("Unclosed Comment",line));

		while (true) {
			if (prog[cursor]=='\n') line++;
			cursor++;
			if ( prog[cursor] =='/' && prog[cursor-1] =='*') {
				cursor++;
				return 0;
				//return new TEndCommand(); // Shure of this?
			}

			if (prog[cursor]=='\0')
				throw(new Error("Unclosed Comment",line));

		}
	}
	// SingleLine Comment
	// Returns 0 if a comment is read.
	else  if (c=='/'&& prog[cursor+1]=='/') {
		cursor++;
		while (true) {
			if (prog[cursor]=='\0'||prog[cursor]=='\n')
				return 0;
			cursor++;
		}
	}
	// Numbers
	else if (isNumber(c)) {
		int to=cursor;
		double number=StringUtils::getNumber(prog,to);
		if (to>cursor && !isChar(prog[to])) {
			cursor=to;
			return new TObject(Number::create(number));
		} else {
			std::cout << number ;
			throw(new Error(  string("Syntax Error in Number."),line));
		}

		// Identifiers, Control commands, true/false
	} else if (isChar(c)) {
	std::string accum;
		while ( isNumber(c) || isChar(c)) {
			accum+=c;
			cursor++;
			c=prog[cursor];
		}
		identifierId id=EScript::stringToIdentifierId(accum);
		Token * o=identifyToken(id);
		if (o!=NULL) {
			return o->clone();
		}else if (id==Consts::IDENTIFIER_LINE) { // __LINE__
			return new TObject(Number::create(line));
		}  else  {
			const Operator *op=Operator::getOperator(id);
			if (op!=NULL)
				return new TOperator(op);
			return new TIdentifier(id);
		}
	} else if (c==';') {
		cursor++;
		return new TEndCommand();
	} else if (c=='{') {
		cursor++;
		return new TStartBlock();
	} else if (c=='}') {
		cursor++;
		return new TEndBlock();
	} else if (c=='(') {
		cursor++;
		return new TStartBracket();
	} else if (c==')') {
		cursor++;
		return new TEndBracket();
	} else if (c==',') {
		cursor++;
		return new TDelimiter();
	} else if (c=='[') {
		cursor++;
		return new TStartIndex();
	} else if (c==']') {
		cursor++;
		return new TEndIndex();
	} else if (c==':' && prog[cursor+1]!='=' && prog[cursor+1]!=':' ) {
		cursor++;
		return new TColon();
	} else if (c=='$' && isChar(prog[cursor+1]) ){
	c=prog[++cursor]; // consume '$'
	std::string accum;
		while ( isNumber(c) || isChar(c)) {
			accum+=c;
			cursor++;
			c=prog[cursor];
		}
//        std::cout << "FOUND ID :"<<accum<<":"<<cursor<<"\n";
		return new TObject(Identifier::create(accum));

	} else if ( isOperator(c) ) {
		int i=cursor;
		std::string accum;
		while (isOperator(c)) {
			accum+=c;
			i++;
			c=prog[i];
			//  if(accum!="-"&& c=='-')
			//      break;
		}
		int size=accum.size();
		const Operator * op=NULL;
		while (true) {
			string ops=accum.substr(0,size);
			op=Operator::getOperator(ops);
			if (op!=NULL) {
				cursor+=size;
				//if(size>1) cursor--;
				break;
			}
			size--;
			if (size<=0) {
				std::cout  << std::endl<< accum << std::endl;
				throw(new Error(  string("Unknown Operator: ")+accum,line));
			}
		}
		// test for unary minus
		if (op->getString()=="-") {
			Token * last=tokens.size()>0?tokens[tokens.size()-1]:NULL; // Bugfix[BUG:20090107]
			if ( last==NULL ||
					(!(dynamic_cast<TEndBracket *>(last) || dynamic_cast<TEndIndex *>(last)||
					   dynamic_cast<TIdentifier *>(last) || dynamic_cast<TObject *>(last)))){
//					dynamic_cast<Number *>(last)|| dynamic_cast<String *>(last)||dynamic_cast<Bool *>(last)))){
				// TODO ++,--

				op=Operator::getOperator("_-");
			}
		}

		return new TOperator(op);
	}
	// String: ".*" | '.*'
	else if (c=='"' || c=='\'') {
		char stringEncloser=c;

		cursor++;
		c=prog[cursor];
		std::ostringstream s;

		while (c!='\0' && c!= stringEncloser) {
			if (c=='\n')
				line++;
			if (c=='\\' ) { // http://de.wikipedia.org/wiki/Steuerzeichen
				cursor++;
				c=prog[cursor];
				if (c==0)
					throw(new Error(  string("Unclosed String. 1")+s.str().substr(0,10),line));
				else if (c=='0') // NULL
					c='\0';
				else if (c=='a') // BELL
					c='\a';
				else if (c=='b') // BACKSPACE
					c='\b';
				else if (c=='n') // LINEFEED
					c='\n';
				else if (c=='r') // CARRIAGE RETURN
					c='\r';
				else if (c=='t') // TAB
					c='\t';
				else if (c=='\\')
					c='\\';
				else if (c=='"')
					c='\"';
				else if (c=='\'')
					c='\'';
			}
			s<<c;
			cursor++;
			c=prog[cursor];
		}
		if (c=='\0')
			throw(new Error(  string("Unclosed String. 2")+s.str().substr(0,10),line));
		cursor++;
		return new TObject(String::create(s.str()));
	}
//        else if (c=='.') {
//            //cursor++;
//            return new TDot();
//        }
	//    cursor++;
	//std::cout << "\n"<<(int)(prog+cursor);
	std::cout << " \a !!!";
	throw(new Error(  string("Unknown Syntax Error near \n...")+(prog+ (cursor>10?(cursor-10):0) ),line));
	return new Token();
}

}
