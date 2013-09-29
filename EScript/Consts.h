// Consts.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_CONSTS_H_INCLUDED
#define ES_CONSTS_H_INCLUDED

#include "Utils/StringId.h"

namespace EScript{

//! Common identifiers and other numerical constants
//	@{
struct Consts{
	static const StringId FILENAME_INLINE;

	static const StringId IDENTIFIER_attr_printableName;
	static const StringId IDENTIFIER_this;
	static const StringId IDENTIFIER_thisFn;
	static const StringId IDENTIFIER_internalResult;
	static const StringId IDENTIFIER_fn_call;
	static const StringId IDENTIFIER_fn_constructor;
	static const StringId IDENTIFIER_fn_greater;
	static const StringId IDENTIFIER_fn_it_end;
	static const StringId IDENTIFIER_fn_it_next;
	static const StringId IDENTIFIER_fn_it_key;
	static const StringId IDENTIFIER_fn_it_value;
	static const StringId IDENTIFIER_fn_less;
	static const StringId IDENTIFIER_fn_equal;
	static const StringId IDENTIFIER_fn_identical;
	static const StringId IDENTIFIER_fn_getIterator;
	static const StringId IDENTIFIER_fn_get;
	static const StringId IDENTIFIER_fn_set;

	static const StringId IDENTIFIER_true;
	static const StringId IDENTIFIER_false;
	static const StringId IDENTIFIER_void;
	static const StringId IDENTIFIER_null;

	static const StringId IDENTIFIER_as;
	static const StringId IDENTIFIER_break;
	static const StringId IDENTIFIER_catch;
	static const StringId IDENTIFIER_case;
	static const StringId IDENTIFIER_continue;
	static const StringId IDENTIFIER_default;
	static const StringId IDENTIFIER_do;
	static const StringId IDENTIFIER_else;
	static const StringId IDENTIFIER_exit;
	static const StringId IDENTIFIER_for;
	static const StringId IDENTIFIER_foreach;
	static const StringId IDENTIFIER_if;
	static const StringId IDENTIFIER_namespace;
	static const StringId IDENTIFIER_return;
	static const StringId IDENTIFIER_switch;
	static const StringId IDENTIFIER_throw;
	static const StringId IDENTIFIER_try;
	static const StringId IDENTIFIER_var;
	static const StringId IDENTIFIER_while;
	static const StringId IDENTIFIER_yield;

	static const StringId IDENTIFIER_LINE;

	static const StringId ANNOTATION_ATTR_const;
	static const StringId ANNOTATION_ATTR_init;
	static const StringId ANNOTATION_ATTR_member;
	static const StringId ANNOTATION_ATTR_override;
	static const StringId ANNOTATION_ATTR_private;
	static const StringId ANNOTATION_ATTR_public;
	static const StringId ANNOTATION_ATTR_type;
	
	static const StringId ANNOTATION_FN_super;

	static const StringId ANNOTATION_STMT_once;

	static const size_t LOCAL_VAR_INDEX_this = 0; // $0
	static const size_t LOCAL_VAR_INDEX_thisFn = 1;// $1
	static const size_t LOCAL_VAR_INDEX_internalResult = 2; // $2
	static const size_t LOCAL_VAR_INDEX_firstParameter = 3; // $3

	static const size_t SYS_CALL_CREATE_ARRAY = 0;
	static const size_t SYS_CALL_CREATE_MAP = 1;
	static const size_t SYS_CALL_THROW_TYPE_EXCEPTION = 2;
	static const size_t SYS_CALL_THROW = 3;
	static const size_t SYS_CALL_EXIT = 4;
	static const size_t SYS_CALL_GET_ITERATOR = 5;
	static const size_t SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS = 6;
	static const size_t SYS_CALL_EXPAND_PARAMS_ON_STACK = 7;
	static const size_t SYS_CALL_CASE_TEST = 8;
	static const size_t SYS_CALL_ONCE = 9;
	static const size_t NUM_SYS_CALLS = 10;

	static const uint32_t DYNAMIC_PARAMETER_COUNT = ~0u;
};
//	@}


}
#endif // ES_CONSTS_H_INCLUDED
