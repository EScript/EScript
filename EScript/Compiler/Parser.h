// Parser.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Tokenizer.h"

#include "../Utils/CodeFragment.h"
#include "../Utils/Logger.h"

#include <vector>

#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

namespace EScript {
namespace AST{
class Block;
}

//! [Parser]
class Parser {
	public:
		Parser(Logger * logger = nullptr);
		ERef<AST::Block> parse(const CodeFragment & code);
	private:
		_CountedRef<Logger> logger;

};
}

#endif // PARSER_H
