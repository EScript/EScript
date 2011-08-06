// Token.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TOKENS_H
#define TOKENS_H

#include "../Objects/Internals/Block.h"
#include "../Objects/Object.h"
#include "../Utils/EReferenceCounter.h"
#include "../Utils/Hashing.h"
#include "../Utils/ObjRef.h"
#include "Operators.h"

#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include <string>

namespace EScript {

struct TokenReleaseHandler;

/*! [Token] */
class Token:public EReferenceCounter<Token,TokenReleaseHandler> {
	public:
		static const uint32_t TYPE_ID=0x00;
		static uint32_t getTypeId()			{	return 0x00;	}

		template<class TokenType_t>
		static bool isA(Token * t){
			return t==NULL ? false : ( (TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
		}
		template<class TokenType_t>
		static bool isA(const _CountedRef<Token> & t){
			return t.isNull() ? false : ( (TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
		}
		template<class TokenType_t>
		static TokenType_t * cast(const _CountedRef<Token>  & t){
			return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t.get()) :  NULL;
		}
		template<class TokenType_t>
		static TokenType_t * cast(Token * t){
			return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t) : NULL ;
		}
// --------------

		static int tokenCount;

		Token(const uint32_t _type=getTypeId()) :
				typeId(_type),line(0),startingPos(std::string::npos) 		{	tokenCount++;	}
		virtual ~Token() 					{	tokenCount--;	}
		virtual std::string toString()const 		{	return std::string("Token");	}

		void setLine(int _line) 			{	line=_line;	}
		int getLine()const					{	return line;	}

		// ---o
		virtual Token * clone()const       	{   return new Token();   }

		uint32_t getType()const				{	return typeId;	}
		const uint32_t typeId;

		void setStaringPos(size_t p)		{	startingPos = p;	}
		size_t getStartingPos()const		{	return startingPos; }

	private:
		int line;
		size_t startingPos;
};
// -----
class TIdentifier :  public Token {
	private:
		identifierId id;
	public:
		static const uint32_t TYPE_ID=0x01 << 0;
		static uint32_t getTypeId()			{	return TYPE_ID;	}

		TIdentifier(identifierId _id) : Token(getTypeId()),id(_id)	{	 }
		virtual std::string toString()const      {  	return identifierIdToString(id);	}

		identifierId getId()const          	{   return id;     }
		virtual Token * clone()const		{   return new TIdentifier(id);   }

};

// -----
class TControl :  public Token {
	public:
		static const uint32_t TYPE_ID=0x01 << 1;
		static uint32_t getTypeId()			{	return TYPE_ID;	}
		TControl(const char * _name) : id(stringToIdentifierId(_name)) {   }
		TControl(identifierId _id) : Token(getTypeId()),id(_id) {   }
		virtual std::string toString()const  	{   return identifierIdToString(id);    }
		identifierId getId()const           {   return id;    }
		virtual Token * clone()const		{   return new TControl(id);  }
	private:
		identifierId id;
};
// -----
struct TEndCommand :  public Token {
	static const uint32_t TYPE_ID=0x01 << 2;
	TEndCommand() : Token(getTypeId()) 	{   }
	virtual std::string toString()const		{	return ";";	}
	virtual Token * clone()const 		{	return new TEndCommand();	}

	static uint32_t getTypeId()			{	return TYPE_ID;	}
};

// -----
struct TEndScript :  public Token	{
	static const uint32_t TYPE_ID=0x01 << 3;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TEndScript() : Token(getTypeId()) 	{   }
	virtual std::string toString()const 		{	return "EndScript";	}
	virtual Token * clone()const 		{	return new TEndScript();	}
};

// -----
struct TStartBlock :  public Token {
	static const uint32_t TYPE_ID=0x01 << 4;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TStartBlock(Block * _block=NULL) : Token(getTypeId()),block(_block) {}
	void setBlock(Block * _block)    	{   block=_block;   }
	Block * getBlock()const          	{   return block.get();   }
	virtual std::string toString()const  	{   return "{"; }
	virtual Token * clone()const   		{   return new TStartBlock(block.get());  }

	private:
		ERef<Block> block;
};

// -----
struct TEndBlock :  public Token {
	static const uint32_t TYPE_ID=0x01 << 5;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TEndBlock() : Token(getTypeId()) 	{   }
	virtual std::string toString()const 		{	return "}";	}
	virtual Token * clone()const 		{	return new TEndBlock();	}
};
// -----

struct TStartMap :  public Token {
	static const uint32_t TYPE_ID=0x01 << 6;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TStartMap()  : Token(getTypeId()) 	{	}
	virtual std::string toString()const 		{	return "_{";	}
	virtual Token * clone()const 		{	return new TStartMap();	}
};

// -----
struct TEndMap :  public Token {
	static const uint32_t TYPE_ID=0x01 << 7;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TEndMap()  : Token(getTypeId()) 	{	}
	virtual std::string toString()const 		{	return "}_";	}
	virtual Token * clone()const 		{	return new TEndMap();	}
};
// -----
struct TMapDelimiter :  public Token {
	static const uint32_t TYPE_ID=0x01 << 8;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TMapDelimiter()  : Token(getTypeId()) {	}
	virtual std::string toString()const 		{	return "_:_";	}
	virtual Token * clone()const 		{	return new TMapDelimiter();	}
};


// -----
struct TColon :  public Token {
	static const uint32_t TYPE_ID=0x01 << 9;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TColon()  : Token(getTypeId()) 		{	}
	virtual std::string toString()const 		{	return ":";	}
	virtual Token * clone()const 		{	return new TColon();	}
};


// -----
struct TObject :  public Token {
	static const uint32_t TYPE_ID=0x01 << 10;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TObject(Object * _obj)  : Token(getTypeId()),obj(_obj) {}
	virtual std::string toString()const 		{	return obj.toString();	}
	virtual Token * clone()const 		{	return new TObject(obj->clone());	}
	ObjRef obj;
};

// -----
struct TOperator :  public Token {
	static const uint32_t TYPE_ID=0x01 << 11;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TOperator(const std::string & s,const int _type=getTypeId())  : Token(_type)	{	op=Operator::getOperator(s);	}
	TOperator(int id)  : Token(getTypeId())					{	op=Operator::getOperator(id);	}
	TOperator(const Operator * _op) : Token(getTypeId()),op(_op) {}

	int getPrecedence() 				{	return op->getPrecedence();	}
	int getAssociativity() 				{	return op->getAssociativity();	}
	const Operator * getOperator()const {	return op;	}

	virtual std::string toString()const 		{	return op->getString();	}
	virtual Token * clone()const 		{	return new TOperator(op);	}

	private:
		const Operator * op;
};


// -----
struct TStartBracket :  public TOperator {
	static const uint32_t TYPE_ID=0x01 << 12 | TOperator::TYPE_ID;
	static uint32_t getTypeId()			{	return TYPE_ID;	}

	size_t endBracketIndex;
	TStartBracket() : TOperator("(",getTypeId()),endBracketIndex(0) 	{}
	virtual Token * clone()const 		{	return new TStartBracket();	}
};

// -----
struct TEndBracket :  public TOperator { // Token: there may be a reason why TEndBrakcet should directly inherit from Token!?!
	static const uint32_t TYPE_ID=0x01 << 13 | TOperator::TYPE_ID;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TEndBracket() : TOperator(")",getTypeId()) 		{}
//    virtual std::string toString()const 		{	return ")";	}
	virtual Token * clone()const 		{	return new TEndBracket();	}
};
// -----
struct TDelimiter :  public TOperator {
	static const uint32_t TYPE_ID=0x01 << 14 | TOperator::TYPE_ID;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TDelimiter() : TOperator(",",getTypeId()) 		{}
	virtual Token * clone()const 		{	return new TDelimiter();	}
};

// -----
struct TStartIndex :public TOperator {
	static const uint32_t TYPE_ID=0x01 << 15 | TOperator::TYPE_ID;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TStartIndex() : TOperator("[",getTypeId()) 		{}
	virtual Token * clone()const 		{	return new TStartIndex();	}
};
// -----
struct TEndIndex :public TOperator {
	static const uint32_t TYPE_ID=0x01 << 16 | TOperator::TYPE_ID;
	static uint32_t getTypeId()			{	return TYPE_ID;	}
	TEndIndex() : TOperator("]",getTypeId()) 		{}
	virtual Token * clone()const 		{	return new TEndIndex();	}
};
struct TokenReleaseHandler{
	static void release(Token * t){
		delete t;
	}
};


}

#endif // TOKENS_H
