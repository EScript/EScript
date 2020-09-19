// Tokenizer.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include "../Objects/Exception.h"
#include "../Utils/StringId.h"
#include "../Utils/ObjRef.h"

#include <cstddef>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace EScript {

//! [Tokenizer]
class Tokenizer {
	public:
		typedef std::unordered_map<StringId, _CountedRef<Token> > tokenMap_t;
		typedef std::vector<_CountedRef<Token> > tokenList_t;
		ESCRIPTAPI static Token * identifyStaticToken(StringId id);

		//!	[Tokenizer::Error] ---|> [Exception] ---|> [Object]
		class Error : public Exception {
			public:
				explicit Error(const std::string & s,int _line=-1):Exception(std::string("Tokenizer: ")+s) {
					setLine(_line);
				}
		};
		// ---

		ESCRIPTAPI void getTokens( const std::string & codeU8,tokenList_t & tokens);
		ESCRIPTAPI void defineToken(const std::string & name,Token * value);

	private:

		ESCRIPTAPI Token * readNextToken(const std::string & codeU8, std::size_t & cursor,int &line,size_t & startPos,tokenList_t & tokens);
		ESCRIPTAPI Token * identifyToken(StringId id)const;

		static bool isNumber(const char c)	{	return c>='0' && c<='9';	}
		static bool isChar(char c)			{	return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || c<0; }
		static bool isWhitechar(char c)		{	return (c=='\n'||c==' '||c=='\t'||c==13||c==3);	}
		static bool isOperator(char c)		{	return strchr("+-/*|%&!<>=^.?:~@",c)!=nullptr;	}

		tokenMap_t customTokens;
};

}

#endif // TOKENIZER_H
