// AnnotatedStatement.h
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
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
		virtual ~AnnotatedStatement() {}

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
