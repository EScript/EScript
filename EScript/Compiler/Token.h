// Token.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef TOKENS_H
#define TOKENS_H

#include "AST/Block.h"
#include "../Utils/EReferenceCounter.h"
#include "../Utils/Hashing.h"
#include "../Utils/ObjRef.h"
#include "Operators.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <string>

namespace EScript {

//! [Token]
class Token:public EReferenceCounter<Token> {
	public:
		static const uint32_t TYPE_ID = 0x00;
		static uint32_t getTypeId()			{	return 0x00;	}

		template<class TokenType_t>
		static bool isA(Token * t){
			return t==nullptr ? false : ( (TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
		}
		template<class TokenType_t>
		static bool isA(const _CountedRef<Token> & t){
			return t.isNull() ? false : ( (TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
		}
		template<class TokenType_t>
		static TokenType_t * cast(const _CountedRef<Token> & t){
			return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t.get()) : nullptr;
		}
		template<class TokenType_t>
		static TokenType_t * cast(Token * t){
			return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t) : nullptr ;
		}
// --------------

		static int tokenCount;

		Token(const uint32_t _type = getTypeId()) :
				typeId(_type),line(0),startingPos(std::string::npos)		{	++tokenCount;	}
		virtual ~Token()					{	tokenCount--;	}
		virtual std::string toString()const	{	return std::string("Token");	}

		void setLine(int _line)				{	line = _line;	}
		int getLine()const					{	return line;	}

		// ---o
		virtual Token * clone()const	 	{	return new Token;	}

		uint32_t getType()const				{	return typeId;	}
		const uint32_t typeId;

		void setStaringPos(size_t p)		{	startingPos = p;	}
		size_t getStartingPos()const		{	return startingPos; }

	private:
		int line;
		size_t startingPos;
};
// -----
class TIdentifier : public Token {
	private:
		StringId id;
	public:
		static const uint32_t TYPE_ID = 0x01 << 0;
		static uint32_t getTypeId()				{	return TYPE_ID;	}

		TIdentifier(StringId _id) : Token(getTypeId()),id(_id)	{	 }
		std::string toString()const	override	{	return id.toString();	}

		StringId getId()const					{	return id;	 }
		Token * clone()const override			{	return new TIdentifier(id);	}

};

// -----
class TControl : public Token {
	public:
		static const uint32_t TYPE_ID = 0x01 << 1;
		static uint32_t getTypeId()				{	return TYPE_ID;	}
		TControl(const char * _name) : id(_name)	{	}
		TControl(StringId _id) : Token(getTypeId()),id(_id) {	}
		std::string toString()const override	{	return id.toString();	}
		StringId getId()const		 			{	return id;	}
		Token * clone()const override			{	return new TControl(id);	}
	private:
		StringId id;
};
// -----
struct TEndCommand : public Token {
	static const uint32_t TYPE_ID = 0x01 << 2;
	TEndCommand() : Token(getTypeId())			{	}
	std::string toString()const override		{	return ";";	}
	Token * clone()const override				{	return new TEndCommand;	}

	static uint32_t getTypeId()					{	return TYPE_ID;	}
};

// -----
struct TEndScript : public Token	{
	static const uint32_t TYPE_ID = 0x01 << 3;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TEndScript() : Token(getTypeId())			{	}
	std::string toString()const override		{	return "EndScript";	}
	Token * clone()const override				{	return new TEndScript;	}
};

// -----
struct TStartBlock : public Token {
	static const uint32_t TYPE_ID = 0x01 << 4;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TStartBlock(AST::Block * _block = nullptr) : Token(getTypeId()),block(_block) {}
	void setBlock(AST::Block * _block)			{	block = _block;	}
	AST::Block * getBlock()const				{	return block.get();	}
	std::string toString()const override		{	return "{"; }
	Token * clone()const override 				{	return new TStartBlock(block.get());	}

	private:
		ERef<AST::Block> block;
};

// -----
struct TEndBlock : public Token {
	static const uint32_t TYPE_ID = 0x01 << 5;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TEndBlock() : Token(getTypeId())			{	}
	std::string toString()const override		{	return "}";	}
	Token * clone()const override				{	return new TEndBlock;	}
};
// -----

struct TStartMap : public Token {
	static const uint32_t TYPE_ID = 0x01 << 6;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TStartMap() : Token(getTypeId())			{	}
	std::string toString()const override		{	return "_{";	}
	Token * clone()const override				{	return new TStartMap;	}
};

// -----
struct TEndMap : public Token {
	static const uint32_t TYPE_ID = 0x01 << 7;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TEndMap() : Token(getTypeId())				{	}
	std::string toString()const override		{	return "}_";	}
	Token * clone()const override				{	return new TEndMap;	}
};
// -----
struct TMapDelimiter : public Token {
	static const uint32_t TYPE_ID = 0x01 << 8;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TMapDelimiter() : Token(getTypeId())		{	}
	std::string toString()const override		{	return "_:_";	}
	Token * clone()const override				{	return new TMapDelimiter;	}
};


// -----
struct TColon : public Token {
	static const uint32_t TYPE_ID = 0x01 << 9;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TColon() : Token(getTypeId())				{	}
	std::string toString()const override		{	return ":";	}
	Token * clone()const override				{	return new TColon;	}
};

// -----
struct TOperator : public Token {
	static const uint32_t TYPE_ID = 0x01 << 11;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TOperator(const std::string & s,const uint32_t _type = getTypeId()) : Token(_type)	{	op = Operator::getOperator(s);	}
	TOperator(StringId id) : Token(getTypeId())	{	op = Operator::getOperator(id);	}
	TOperator(const Operator * _op) : Token(getTypeId()),op(_op) {}

	int getPrecedence()							{	return op->getPrecedence();	}
	int getAssociativity()						{	return op->getAssociativity();	}
	const Operator * getOperator()const			{	return op;	}

	std::string toString()const override		{	return op->getString();	}
	Token * clone()const override				{	return new TOperator(op);	}

	private:
		const Operator * op;
};


// -----
struct TStartBracket : public TOperator {
	static const uint32_t TYPE_ID = 0x01 << 12 | TOperator::TYPE_ID;
	static uint32_t getTypeId()					{	return TYPE_ID;	}

	size_t endBracketIndex;
	TStartBracket() : TOperator("(",getTypeId()),endBracketIndex(0)	{}
	Token * clone()const override				{	return new TStartBracket;	}
};

// -----
struct TEndBracket : public TOperator { // Token: there may be a reason why TEndBrakcet should directly inherit from Token!?!
	static const uint32_t TYPE_ID = 0x01 << 13 | TOperator::TYPE_ID;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TEndBracket() : TOperator(")",getTypeId())	{}
//	std::string toString()const override		{	return ")";	}
	Token * clone()const override				{	return new TEndBracket;	}
};
// -----
struct TDelimiter : public TOperator {
	static const uint32_t TYPE_ID = 0x01 << 14 | TOperator::TYPE_ID;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TDelimiter() : TOperator(",",getTypeId())	{}
	Token * clone()const override				{	return new TDelimiter;	}
};

// -----
struct TStartIndex :public TOperator {
	static const uint32_t TYPE_ID = 0x01 << 15 | TOperator::TYPE_ID;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TStartIndex() : TOperator("[",getTypeId())	{}
	Token * clone()const override				{	return new TStartIndex;	}
};
// -----
struct TEndIndex :public TOperator {
	static const uint32_t TYPE_ID = 0x01 << 16 | TOperator::TYPE_ID;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TEndIndex() : TOperator("]",getTypeId())	{}
	Token * clone()const override				{	return new TEndIndex;	}
};
// -----
struct TValueBool : public Token {
	static const uint32_t TYPE_ID = 0x01 << 17;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TValueBool(bool v) : Token(getTypeId()), value(v){	}
	std::string toString()const override		{	return value ? "true" : "false";	}
	Token * clone()const override				{	return new TValueBool(value);	}
	bool getValue()const						{	return value;	}

	bool value;
};
struct TValueIdentifier : public Token {
	static const uint32_t TYPE_ID = 0x01 << 18;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TValueIdentifier(const StringId & v) : Token(getTypeId()), value(v){	}
	std::string toString()const override		{	return value.toString();	}
	Token * clone()const override				{	return new TValueIdentifier(value);	}
	const StringId & getValue()const			{	return value;	}

	StringId value;
};
struct TValueNumber : public Token {
	static const uint32_t TYPE_ID = 0x01 << 19;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TValueNumber(double v) : Token(getTypeId()), value(v){	}
	std::string toString()const override		{	return "Number";	}
	Token * clone()const override				{	return new TValueNumber(value);	}
	double getValue()const						{	return value;	}

	double value;
};
struct TValueString : public Token {
	static const uint32_t TYPE_ID = 0x01 << 20;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TValueString(const std::string &v) : Token(getTypeId()), value(v){	}
	std::string toString()const override		{	return value;	}
	Token * clone()const override				{	return new TValueString(value);	}
	const std::string & getValue()const			{	return value;	}
	void setString(const std::string & s)		{	value = s;	}

	std::string value;
};
struct TValueVoid : public Token {
	static const uint32_t TYPE_ID = 0x01 << 21;
	static uint32_t getTypeId()					{	return TYPE_ID;	}
	TValueVoid() : Token(getTypeId())			{	}
	std::string toString()const override		{	return "void";	}
	Token * clone()const override				{	return new TValueVoid;	}
};

}

#endif // TOKENS_H
