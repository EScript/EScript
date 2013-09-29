// AnnotatedStatement.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_ANNOTATED_STATEMENT_H
#define ES_ANNOTATED_STATEMENT_H

#include "ASTNode.h"

namespace EScript {
namespace AST {

//! [AnnotatedStatement]  ---|> [ASTNode]
class AnnotatedStatement : public ASTNode {
		ES_PROVIDES_TYPE_NAME(AnnotatedStatement)
	public:

		AnnotatedStatement( ptr_t _statement,
							const StringId & _annotationName) :
			ASTNode(TYPE_ANNOTATED_STATEMENT,false),
			statement(_statement),
			annotationName(_annotationName){}
		virtual ~AnnotatedStatement() = default;

		StringId getName()const							{	return annotationName;	}
		ptr_t getStatement()const						{	return statement.get();	}

	private:
		ref_t statement;
		StringId annotationName;
		// add annotation parameters when needed
};
}
}

#endif // ES_ANNOTATED_STATEMENT_H
