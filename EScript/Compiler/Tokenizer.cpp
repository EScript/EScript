// Tokenizer.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Tokenizer.h"

#include "../Consts.h"
#include "../Utils/StringUtils.h"
#include "Operators.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace EScript {

//! (static)
Token * Tokenizer::identifyStaticToken(StringId id){
	static tokenMap_t constants;
	// init
	if(constants.empty()){
		constants[Consts::IDENTIFIER_if] = new TControl(Consts::IDENTIFIER_if);
		constants[Consts::IDENTIFIER_else] = new TControl(Consts::IDENTIFIER_else);
		constants[Consts::IDENTIFIER_do] = new TControl(Consts::IDENTIFIER_do);

		constants[Consts::IDENTIFIER_while] = new TControl(Consts::IDENTIFIER_while);
		constants[Consts::IDENTIFIER_break] = new TControl(Consts::IDENTIFIER_break);
		constants[Consts::IDENTIFIER_static] = new TControl(Consts::IDENTIFIER_static);
		constants[Consts::IDENTIFIER_var] = new TControl(Consts::IDENTIFIER_var);

		constants[Consts::IDENTIFIER_continue] = new TControl(Consts::IDENTIFIER_continue);
		constants[Consts::IDENTIFIER_return] = new TControl(Consts::IDENTIFIER_return);
		constants[Consts::IDENTIFIER_exit] = new TControl(Consts::IDENTIFIER_exit);
		constants[Consts::IDENTIFIER_foreach] = new TControl(Consts::IDENTIFIER_foreach);
		constants[Consts::IDENTIFIER_as] = new TControl(Consts::IDENTIFIER_as);
		constants[Consts::IDENTIFIER_for] = new TControl(Consts::IDENTIFIER_for);
		constants[Consts::IDENTIFIER_switch] = new TControl(Consts::IDENTIFIER_switch);

		constants[Consts::IDENTIFIER_try] = new TControl(Consts::IDENTIFIER_try);
		constants[Consts::IDENTIFIER_catch] = new TControl(Consts::IDENTIFIER_catch);
		constants[Consts::IDENTIFIER_throw] = new TControl(Consts::IDENTIFIER_throw);
		constants[Consts::IDENTIFIER_yield] = new TControl(Consts::IDENTIFIER_yield);
		constants[Consts::IDENTIFIER_namespace] = new TControl(Consts::IDENTIFIER_namespace);

		constants[Consts::IDENTIFIER_true] = new TValueBool(true);
		constants[Consts::IDENTIFIER_false] = new TValueBool(false);
		constants[Consts::IDENTIFIER_void] = new TValueVoid;
		constants[Consts::IDENTIFIER_null] = new TValueVoid;

	}
	const tokenMap_t::const_iterator it = constants.find(id);
	return it==constants.end() ? nullptr : it->second.get();
}

//! (internal)
Token * Tokenizer::identifyToken(StringId id)const{
	const tokenMap_t::const_iterator it = customTokens.find(id);
	return it==customTokens.end() ? identifyStaticToken(id) : it->second.get();
}

//! (internal)
void Tokenizer::defineToken(const std::string & name,Token * value){
	customTokens[StringId(name)] = value;
}

void Tokenizer::getTokens( const std::string & codeU8,tokenList_t & tokens){
	std::size_t cursor = 0;
	int line = 1;
	size_t startPos = std::string::npos;

	Token * token;
	do {
		token = readNextToken(codeU8,cursor,line,startPos,tokens);
		if(token!=nullptr) {
			token->setLine(line);
			token->setStaringPos(startPos);
			tokens.push_back(token);
		}
	} while(! Token::isA<TEndScript>(token) );

}

static bool strBeginsWith(const char * subject,const char * find) {
	for(size_t i = 0 ; find[i]!=0 ; ++i){
		if(subject[i]!=find[i])
			return false;
	}
	return true;
}
//!	Reads the next Token from prog beginning with position cursor and moves cursor to the next Token.
Token * Tokenizer::readNextToken(const std::string & codeU8, std::size_t & cursor,int &line,size_t & startPos,tokenList_t & tokens) {
	if(cursor>=codeU8.length())
		return new TEndScript;

	const char * prog = codeU8.c_str();
	char c = prog[cursor];

	// Step over whitespace characters
	while( isWhitechar(c) ) {
		if(c=='\n') ++line;
		++cursor;
		if(cursor>=codeU8.length())
			return new TEndScript;

		c = codeU8[cursor];
	}
	startPos = static_cast<size_t>(cursor);

	// Raw strings:   R"Delimiter(my string dum di du)Delimiter"
	if(c=='R' && codeU8[cursor+1]=='"' ) {
		cursor+=2;
		std::ostringstream delimiter;
		delimiter << ')';
		for(c = codeU8[cursor]; c!='('; ++cursor,c = codeU8[cursor]){
			if(cursor>=codeU8.length())
				throw new Error(std::string("Unclosed Raw String; missing '('."),line);
			else if(isWhitechar(c))
				throw new Error(std::string("No whitespace allowed in raw string delimiter."),line);
			delimiter << c;
		}
		delimiter<<'"';
		++cursor; // step over '('

		const std::string d(delimiter.str());

		const auto first = cursor;
		size_t length = 0;
		while(cursor<codeU8.length()){
			c = codeU8[cursor];
			if(c==')'&& strBeginsWith(prog+cursor,d.c_str())){
				cursor+=d.length();
				return new TValueString((std::string(prog+first,length)));
			}else if(c=='\n'){
				++line;
			}
			++cursor;
			++length;
		}
		throw new Error(std::string("Unclosed Raw String; missing '"+d+"'"),line);
	}

	// Multi line comment (returns nullptr)
	if(c=='/' && codeU8[cursor+1]=='*') {
		cursor+=2;
		while(cursor<codeU8.length()) {
			if(codeU8[cursor]=='\n'){
				++line;
			}else if( codeU8[cursor] =='*' && codeU8[cursor+1] =='/') {
				cursor+=2;
				return nullptr;
			}
			++cursor;
		}
		throw new Error("Unclosed Comment",line);
	}
	// Single line comment (returns nullptr)
	else if(c=='/' && codeU8[cursor+1]=='/') {
		++cursor;
		while(cursor<codeU8.length() && codeU8[cursor]!='\n')
			++cursor;
		return nullptr;

	}
	// Numbers
	else if(isNumber(c)) {
		std::size_t to = cursor;
		const double number = StringUtils::readNumber(prog,to);
		if(to>cursor && !isChar(codeU8[to])) {
			cursor = to;
			return new TValueNumber(number);
		} else {
			std::cout << number ;
			throw new Error(  std::string("Syntax Error in Number."),line);
		}

		// Identifiers, Control commands, true/false
	} else if(isChar(c)) {
		std::string accum;
		while( isNumber(c) || isChar(c)) {
			accum+=c;
			++cursor;
			c = codeU8[cursor];
		}
		const StringId id(accum);
		Token * o = identifyToken(id);
		if(o!=nullptr) {
			return o->clone();
		}else if(id==Consts::IDENTIFIER_LINE) { // __LINE__
			return new TValueNumber(line);
		}  else  {
			const Operator *op = Operator::getOperator(id);
			if(op!=nullptr)
				return new TOperator(op);
			return new TIdentifier(id);
		}
	} else if(c==';') {
		++cursor;
		return new TEndCommand;
	} else if(c=='{') {
		++cursor;
		return new TStartBlock;
	} else if(c=='}') {
		++cursor;
		return new TEndBlock;
	} else if(c=='(') {
		++cursor;
		return new TStartBracket;
	} else if(c==')') {
		++cursor;
		return new TEndBracket;
	} else if(c==',') {
		++cursor;
		return new TDelimiter;
	} else if(c=='[') {
		++cursor;
		return new TStartIndex;
	} else if(c==']') {
		++cursor;
		return new TEndIndex;
	} else if(c==':' && codeU8[cursor+1]!='=' && codeU8[cursor+1]!=':' ) {
		++cursor;
		return new TColon;
	} else if(c=='$' && isChar(codeU8[cursor+1]) ){
		c = codeU8[++cursor]; // consume '$'
		std::string accum;
		while( isNumber(c) || isChar(c)) {
			accum+=c;
			++cursor;
			c = codeU8[cursor];
		}
//        std::cout << "FOUND ID :"<<accum<<":"<<cursor<<"\n";
		return new TValueIdentifier(StringId(accum));

	} else if( isOperator(c) ) {
		std::string accum;
		size_t cursor2 = cursor;
		while(isOperator(c)) {
			accum += c;
			++cursor2;
			c = codeU8[cursor2];
		}

		const Operator * op = nullptr;
		for(size_t operatorLength = accum.length(); true; --operatorLength) {
			op = Operator::getOperator(accum.substr(0,operatorLength));
			if(op!=nullptr) {
				cursor+=operatorLength;
				break;
			}
			if(operatorLength<=1) {
				std::cout  << std::endl<< accum << std::endl;
				throw new Error(std::string("Unknown Operator: ")+accum,line);
			}
		}
		// test for unary minus
		if(op->getString()=="-") {
			Token * last = tokens.empty()? nullptr : tokens.back().get(); // Bugfix[BUG:20090107]
			if( last==nullptr ||
					(!(Token::isA<TEndBracket>(last)	|| Token::isA<TEndIndex>(last)||
						Token::isA<TIdentifier>(last)	||
						Token::isA<TValueBool>(last)	|| Token::isA<TValueIdentifier>(last) ||
						Token::isA<TValueString>(last)	|| Token::isA<TValueNumber>(last) ||
						Token::isA<TValueVoid>(last) ))){
				// TODO ++,--

				op = Operator::getOperator("_-");
			}
		}

		return new TOperator(op);
	}
	// String: ".*" | '.*'
	else if(c=='"' || c=='\'') {
		const char stringEncloser = c;
		++cursor;
		c = codeU8[cursor];
		std::ostringstream s;
		while(cursor<codeU8.length()) {
			if(c==stringEncloser){
				++cursor;
				return new TValueString(s.str());
			}else if(c=='\n'){
				++line;
			}else if(c=='\\' ) { // http://de.wikipedia.org/wiki/Steuerzeichen
				++cursor;
				if(cursor>=codeU8.length())
					break;
				switch(codeU8[cursor]){
					case '0':	c = '\0'; break;	// nullptr
					case 'a':	c = '\a'; break;	// BELL
					case 'b':	c = '\b'; break;	// BACKSPACE
					case 'n':	c = '\n'; break;	// LINEFEED
					case 'r':	c = '\r'; break;	// CARRIAGE RETURN
					case 't':	c = '\t'; break;	// TAB
					case '\\':	c = '\\'; break;
					case '"':	c = '\"'; break;
					case '\'':	c = '\''; break;
					default:	c = codeU8[cursor];
				}
			}
			s<<c;
			++cursor;
			c = codeU8[cursor];
		}
		throw new Error(std::string("Unclosed String. 2")+s.str().substr(0,10),line);

	}else if(line==1 && c=='#' && codeU8[cursor+1]=='!') {
		++cursor;
		while(cursor<codeU8.length() && codeU8[cursor]!='\n')
			++cursor;
		return nullptr;

	}
	throw new Error(std::string("Unknown syntax error near: \n...")+(prog+ (cursor>10?(cursor-10):0) ),line);
	return nullptr;
}

}
