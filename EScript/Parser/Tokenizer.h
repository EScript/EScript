// Tokenizer.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include "../Objects/Exception.h"
#include "../Utils/Hashing.h"
#include "../Utils/ObjRef.h"

#include <cstddef>
#include <string>
#include <cstring>
#include <map>
#include <vector>

namespace EScript {

/*! [Tokenizer] */
class Tokenizer {
	public:
		typedef std::map<identifierId, _CountedRef<Token> > tokenMap_t;
		typedef std::vector<_CountedRef<Token> > tokenList_t;
		static Token * identifyStaticToken(identifierId id);

		Tokenizer();
		virtual ~Tokenizer();
		/**
		 *  [Tokenizer::Error] ---|> [Exception] ---|> [Object]
		 */
		class Error:public Exception {
			public:
				Error(std::string s,int _line=-1):Exception(std::string("Tokenizer:")+s) {
					setLine(_line);
				}
		};
		// ---

		void getTokens( const char * prog,tokenList_t & tokens) throw (Exception *);
		void defineToken(const std::string & name,Token * value);

	private:

		Token * readNextToken(const char * prog, int & cursor,int &line,size_t & startPos,tokenList_t & tokens) throw (Exception *);
		Token * identifyToken(identifierId id)const;

		inline bool isNumber(char c) const;
		inline bool isChar(char c) const;
		inline bool isWhitechar(char c) const;
		inline bool isOperator(char c) const;

		tokenMap_t customTokens;
};
// -----------------------------------------------------------------------------
// inline
bool Tokenizer::isNumber(char c) const {
	return c>='0' && c<='9';
}
bool Tokenizer::isChar(char c) const {
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';
}
bool Tokenizer::isWhitechar(char c) const {
	return (c=='\n'||c==' '||c=='\t'||c==13||c==3);
}
bool Tokenizer::isOperator(char c) const {
	return strchr("+-/*|%&!<>=^.?:~@",c)!=NULL;
}
}

#endif // TOKENIZER_H
