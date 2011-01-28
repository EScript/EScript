#ifndef PARSER_H
#define PARSER_H


#include "../Statements/Block.h"
#include "Token.h"
#include "Tokenizer.h"

#include "../Objects/Exception.h"
#include "../Objects/UserFunction.h"

#include <vector>

namespace EScript {

using std::string;
using std::vector;

/*! [Parser] ---|> [Object] */
class Parser : public Object {
        ES_PROVIDES_TYPE_NAME(Parser)
    public:
        static Type* typeObject;
        static void init(EScript::Namespace & globals);

        /*! [Parser::Error] ---|> [Exception] ---|> [Object]    */
        class Error:public Exception {
            public:
                Error(string s,Token * token=NULL):
                    Exception(
                        string("[Parser]")+s,  (token==NULL? -1 : token->getLine())) {
                }
        };
        //-----------

        Parser(Type * type=NULL);
        virtual ~Parser();

        Object * parse(Block * rootBlock,const char * c) throw (Exception *);
        Object * parseFile(Block * rootBlock,const char * filename) throw (Exception *);

        /// ---|> [Object]
        virtual Object * clone()const;

    private:
		// only for debugging
        identifierId currentFilename;
		std::string getCurrentFilename()const	{	return identifierIdToString(currentFilename);	}

        Tokenizer tokenizer;
        void pass_1(Tokenizer::tokenList & tokens)throw (Exception *);
        void pass_2(Tokenizer::tokenList & tokens,Block * root, Tokenizer::tokenList  & enrichedTokens)throw (Exception *);
        //    Object * getStatement(Block * currentBlock, std::vector<Token *> & tokens,int & cursor) throw (Error);
        Object * getControl(Tokenizer::tokenList & tokens,int & cursor) throw (Exception *);
        Object * getExpression(Tokenizer::tokenList & tokens,int & cursor,int to=-1) throw (Exception *);
        Object * getBinaryExpression(Tokenizer::tokenList & tokens,int & cursor,int to) throw (Exception *);
        Object * getBlock(Tokenizer::tokenList & tokens,int & cursor) throw (Exception *) ;
        Object * getMap(Tokenizer::tokenList & tokens,int & cursor) throw (Exception *);
        Object * getFunctionDeclaration(Tokenizer::tokenList & tokens,int & cursor) throw (Exception *);
        UserFunction::parameterList_t * getFunctionParameters(Tokenizer::tokenList & tokens,int & cursor) throw (Exception *);
		void getExpressionsInBrackets(Tokenizer::tokenList & tokens,int & cursor,std::vector<ObjRef> & expressions) throw (Exception *);

		enum lValue_t { LVALUE_NONE, LVALUE_INDEX, LVALUE_MEMBER};
        lValue_t getLValue(Tokenizer::tokenList & tokens,int from,int to,Object * & obj,identifierId & identifier,Object * &indexExpression) throw (Exception *);
        int findExpression(Tokenizer::tokenList & tokens,int cursor);
};
}

#endif // PARSER_H
