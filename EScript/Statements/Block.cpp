#include "Block.h"
#include "../Runtime/Runtime.h"

#include <iostream>
#include <sstream>

using namespace EScript;

/*!	(ctor)	*/
Block::Block(int lineNr):filenameId(0),vars(NULL),line(lineNr) {
    //ctor
}

/*!	(dtor)	*/
Block::~Block() {
    // Delete Statements
    for ( statementCursor c = statements.begin();  c != statements.end(); ++c) {
        Object::removeReference( (*c) );
    }
    delete vars;
    //dtor
}

/*!	---|> [Object]	*/
std::string Block::toString()const {
    static int depth=0;
    std::ostringstream sprinter;
    sprinter << "{" <<  std::endl;
    depth++;
    for ( cStatementCursor c = statements.begin();  c != statements.end(); ++c) {
        if ( (*c) ) {
            for (int i=0;i<depth;++i) sprinter << "\t";
            sprinter <<  (*c)->toString() <<  std::endl;
        }
    }
    depth--;
    for (int i=0;i<depth;++i) sprinter << "\t";
    sprinter << "}";
    return sprinter.str();
}

bool Block::declareVar(identifierId id) {
    if(vars==NULL){
        vars=new declaredVariableMap_t();
    }
    std::pair<declaredVariableMap_t::iterator,bool> result= vars->insert(id);
    return result.second;
}

void Block::addStatement(Object * s) {
    if (s) {
        statements.push_back(s);
        Object::addReference(s);
    }
}

/*!	---|> [Object]	*/
Object * Block::execute(Runtime & rt) {
    return rt.executeBlock(this);
}
