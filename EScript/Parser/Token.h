// Token.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TOKENS_H
#define TOKENS_H

#include "../EScript.h"

#include "../Utils/Hashing.h"
#include "Operators.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>

#ifndef _TokenIsADefined
#define _TokenIsADefined
#define TokenIsA(token,type) (dynamic_cast<type *>(token)!=0)
#endif

namespace EScript {
using std::string;
class Block;
class TokenReleaseHandler;

/*! [Token] */
class Token:public EReferenceCounter<Token,TokenReleaseHandler> {
	public:
		static const int TYPE_ID=0x00;
		static int getTypeId()				{	return 0x00;	}
		template<class TokenType_t>
		static bool isA(Token * t){
			if(t==NULL)
				return false;
			const int & id=TokenType_t::getTypeId();
			return id == t->typeId;
		}

// --------------

		static int tokenCount;

		Token(const int _type=getTypeId()) :
				typeId(_type),line(0) 		{	tokenCount++;	}
		virtual ~Token() 				{	tokenCount--;	}
		virtual string toString()const 	{	return string("Token");	}

		void setLine(int _line) 		{	line=_line;	}
		int getLine()const				{	return line;	}

		virtual Token * clone()const       {   return new Token();   }


		int getType()const{	return typeId;	}
		const int typeId;
	private:
		int line;
};
// -----
class TIdentifier :  public Token {
	private:
		identifierId id;
	public:
		static const int TYPE_ID=0x01;
		static int getTypeId()				{	return TYPE_ID;	}
		TIdentifier(identifierId _id) : Token(getTypeId()),id(_id)	{	 }
		virtual string toString()const      {  	return identifierIdToString(id);	}

		identifierId getId()const          	{   return id;     }
		virtual Token * clone()const       {   return new TIdentifier(id);   }

};

// -----
class TControl :  public Token {
	public:
		static const int TYPE_ID=0x02;
		static int getTypeId()				{	return TYPE_ID;	}
		TControl(const char * _name) : id(stringToIdentifierId(_name)) {   }
		TControl(identifierId _id) : Token(getTypeId()),id(_id) {   }
		virtual string toString()const  	{   return identifierIdToString(id);    }
		identifierId getId()const           {   return id;    }
		virtual Token * clone()const   	{   return new TControl(id);  }
	private:
		identifierId id;
};
// -----
struct TEndCommand :  public Token {
	TEndCommand() : Token(getTypeId()) {   }
	virtual string toString()const		{	return ";";	}
	virtual Token * clone()const 		{	return new TEndCommand();	}

	static const int TYPE_ID=0x03;
	static int getTypeId()				{	return TYPE_ID;	}
};

// -----
struct TEndScript :  public Token	{
	static const int TYPE_ID=0x04;
	static int getTypeId()				{	return TYPE_ID;	}
	TEndScript() : Token(getTypeId()) {   }
	virtual string toString()const 		{	return "EndScript";	}
	virtual Token * clone()const 		{	return new TEndScript();	}
};

// -----
struct TStartBlock :  public Token {
	static const int TYPE_ID=0x05;
	static int getTypeId()				{	return TYPE_ID;	}
	TStartBlock(Block * _block=NULL) : Token(getTypeId()),block(_block) {}
	void setBlock(Block * _block)    	{   block=_block;   }
	Block * getBlock()const          	{   return block.get();   }
	virtual string toString()const  	{   return "{"; }
	virtual Token * clone()const   	{   return new TStartBlock(block.get());  }

	private:
		ERef<Block> block;
};

// -----
struct TEndBlock :  public Token {
	static const int TYPE_ID=0x06;
	static int getTypeId()				{	return TYPE_ID;	}
	TEndBlock() : Token(getTypeId()) {   }
	virtual string toString()const 		{	return "}";	}
	virtual Token * clone()const 		{	 return new TEndBlock();	}
};
// -----

struct TStartMap :  public Token {
	static const int TYPE_ID=0x07;
	static int getTypeId()				{	return TYPE_ID;	}
	TStartMap()  : Token(getTypeId()) {}
	virtual string toString()const 		{	return "_{";	}
	virtual Token * clone()const 		{	return new TStartMap();	}
};

// -----
struct TEndMap :  public Token {
	static const int TYPE_ID=0x08;
	static int getTypeId()				{	return TYPE_ID;	}
	TEndMap()  : Token(getTypeId()) {}
	virtual string toString()const 		{	return "}_";	}
	virtual Token * clone()const 		{	return new TEndMap();	}
};
// -----
struct TMapDelimiter :  public Token {
	static const int TYPE_ID=0x09;
	static int getTypeId()				{	return TYPE_ID;	}
	TMapDelimiter()  : Token(getTypeId()) {}
	virtual string toString()const 		{	return "_:_";	}
	virtual Token * clone()const 		{	return new TMapDelimiter();	}
};


// -----
struct TColon :  public Token {
	static const int TYPE_ID=0x0A;
	static int getTypeId()				{	return TYPE_ID;	}
	TColon()  : Token(getTypeId()) {}
	virtual string toString()const 		{	return ":";	}
	virtual Token * clone()const 		{	return new TColon();	}
};


// -----
struct TObject :  public Token {
	static const int TYPE_ID=0x0B;
	static int getTypeId()				{	return TYPE_ID;	}
	TObject(Object * _obj)  : Token(getTypeId()),obj(_obj) {}
	virtual string toString()const 		{	return obj.toString();	}
	virtual Token * clone()const 		{	return new TObject(obj->clone());	}
	ObjRef obj;
};

// -----
struct TOperator :  public Token {
	static const int TYPE_ID=0x10;
	static int getTypeId()				{	return TYPE_ID;	}
	TOperator(const std::string & s,const int _type=getTypeId())  : Token(_type)	{	op=Operator::getOperator(s);	}
	TOperator(int id)  : Token(getTypeId())					{	op=Operator::getOperator(id);	}
	TOperator(const Operator * _op) : Token(getTypeId()),op(_op) {}

	int getPrecedence() 				{	return op->getPrecedence();	}
	int getAssociativity() 				{	return op->getAssociativity();	}
	const Operator * getOperator()const {	return op;	}

	virtual string toString()const 		{	return op->getString();	}
	virtual Token * clone()const 		{	return new TOperator(op);	}

	private:
		const Operator * op;
};


// -----
struct TStartBracket :  public TOperator {
	static const int TYPE_ID=0x11;
	static int getTypeId()				{	return TYPE_ID;	}

	size_t endBracketIndex;
	TStartBracket() : TOperator("(",getTypeId()),endBracketIndex(0) 	{}
	virtual Token * clone()const 		{	return new TStartBracket();	}
};

// -----
struct TEndBracket :  public TOperator { // Token: there may be a reason why TEndBrakcet should directly inherit from Token!?!
	static const int TYPE_ID=0x12;
	static int getTypeId()				{	return TYPE_ID;	}
	TEndBracket() : TOperator(")",getTypeId()) 		{}
//    virtual string toString()const 		{	return ")";	}
	virtual Token * clone()const 		{	return new TEndBracket();	}
};
// -----
struct TDelimiter :  public TOperator {
	static const int TYPE_ID=0x13;
	static int getTypeId()				{	return TYPE_ID;	}
	TDelimiter() : TOperator(",",getTypeId()) 		{}
	virtual Token * clone()const 		{	return new TDelimiter();	}
};

// -----
struct TStartIndex :public TOperator {
	static const int TYPE_ID=0x14;
	static int getTypeId()				{	return TYPE_ID;	}
	TStartIndex() : TOperator("[",getTypeId()) 		{}
	virtual Token * clone()const 		{	return new TStartIndex();	}
};
// -----
struct TEndIndex :public TOperator {
	static const int TYPE_ID=0x15;
	static int getTypeId()				{	return TYPE_ID;	}
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
