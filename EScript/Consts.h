// Consts.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_CONSTS_H_INCLUDED
#define ES_CONSTS_H_INCLUDED

#include "Utils/StringId.h"

namespace EScript{

//! Common identifiers and other numerical constants
//	@{
struct Consts{
	ESCRIPTAPI static const StringId FILENAME_INLINE;

	ESCRIPTAPI static const StringId IDENTIFIER_attr_printableName;
	ESCRIPTAPI static const StringId IDENTIFIER_this;
	ESCRIPTAPI static const StringId IDENTIFIER_thisFn;
	ESCRIPTAPI static const StringId IDENTIFIER_internalResult;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_call;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_checkConstraint;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_constructor;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_greater;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_it_end;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_it_next;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_it_key;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_it_value;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_less;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_equal;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_identical;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_getIterator;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_get;
	ESCRIPTAPI static const StringId IDENTIFIER_fn_set;

	ESCRIPTAPI static const StringId IDENTIFIER_true;
	ESCRIPTAPI static const StringId IDENTIFIER_false;
	ESCRIPTAPI static const StringId IDENTIFIER_void;
	ESCRIPTAPI static const StringId IDENTIFIER_null;

	ESCRIPTAPI static const StringId IDENTIFIER_as;
	ESCRIPTAPI static const StringId IDENTIFIER_break;
	ESCRIPTAPI static const StringId IDENTIFIER_catch;
	ESCRIPTAPI static const StringId IDENTIFIER_case;
	ESCRIPTAPI static const StringId IDENTIFIER_continue;
	ESCRIPTAPI static const StringId IDENTIFIER_default;
	ESCRIPTAPI static const StringId IDENTIFIER_do;
	ESCRIPTAPI static const StringId IDENTIFIER_else;
	ESCRIPTAPI static const StringId IDENTIFIER_exit;
	ESCRIPTAPI static const StringId IDENTIFIER_for;
	ESCRIPTAPI static const StringId IDENTIFIER_foreach;
	ESCRIPTAPI static const StringId IDENTIFIER_if;
	ESCRIPTAPI static const StringId IDENTIFIER_namespace;
	ESCRIPTAPI static const StringId IDENTIFIER_return;
	ESCRIPTAPI static const StringId IDENTIFIER_static;
	ESCRIPTAPI static const StringId IDENTIFIER_switch;
	ESCRIPTAPI static const StringId IDENTIFIER_throw;
	ESCRIPTAPI static const StringId IDENTIFIER_try;
	ESCRIPTAPI static const StringId IDENTIFIER_var;
	ESCRIPTAPI static const StringId IDENTIFIER_while;
	ESCRIPTAPI static const StringId IDENTIFIER_yield;

	ESCRIPTAPI static const StringId IDENTIFIER_LINE;

	ESCRIPTAPI static const StringId ANNOTATION_ATTR_const;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_init;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_member;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_override;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_private;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_public;
	ESCRIPTAPI static const StringId ANNOTATION_ATTR_type;

	ESCRIPTAPI static const StringId ANNOTATION_FN_super;

	ESCRIPTAPI static const StringId ANNOTATION_STMT_once;

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
	static const size_t SYS_CALL_GET_STATIC_VAR = 10;
	static const size_t SYS_CALL_SET_STATIC_VAR = 11;
	static const size_t NUM_SYS_CALLS = 12;

	static const uint32_t DYNAMIC_PARAMETER_COUNT = ~0u;
};
//	@}


}
#endif // ES_CONSTS_H_INCLUDED
