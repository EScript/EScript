// Consts.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONSTS_H_INCLUDED
#define ES_CONSTS_H_INCLUDED

#include "Objects/Object.h"

namespace EScript{

/*! Common identifiers */
//	@{
struct Consts{
	static const identifierId IDENTIFIER_this;
	static const identifierId IDENTIFIER_thisFn;
	static const identifierId IDENTIFIER_fn_constructor;
	static const identifierId IDENTIFIER_fn_greater;
	static const identifierId IDENTIFIER_fn_less;
	static const identifierId IDENTIFIER_fn_equal;
	static const identifierId IDENTIFIER_fn_identical;
	static const identifierId IDENTIFIER_fn_getIterator;
	static const identifierId IDENTIFIER_fn_get;
	static const identifierId IDENTIFIER_fn_set;

	static const identifierId IDENTIFIER_true;
	static const identifierId IDENTIFIER_false;
	static const identifierId IDENTIFIER_void;
	static const identifierId IDENTIFIER_null;

	static const identifierId IDENTIFIER_as;
	static const identifierId IDENTIFIER_break;
	static const identifierId IDENTIFIER_catch;
	static const identifierId IDENTIFIER_continue;
	static const identifierId IDENTIFIER_do;
	static const identifierId IDENTIFIER_else;
	static const identifierId IDENTIFIER_exit;
	static const identifierId IDENTIFIER_for;
	static const identifierId IDENTIFIER_foreach;
	static const identifierId IDENTIFIER_if;
	static const identifierId IDENTIFIER_namespace;
	static const identifierId IDENTIFIER_return;
	static const identifierId IDENTIFIER_throw;
	static const identifierId IDENTIFIER_try;
	static const identifierId IDENTIFIER_var;
	static const identifierId IDENTIFIER_while;
	static const identifierId IDENTIFIER_yield;

	static const identifierId IDENTIFIER_originalId;

	static const identifierId IDENTIFIER_LINE;
};
//	@}


}
#endif // ES_CONSTS_H_INCLUDED
