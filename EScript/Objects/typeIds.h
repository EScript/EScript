// typeIds.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_TYPEIDS_H_INCLUDED
#define ES_TYPEIDS_H_INCLUDED

#include <stdint.h>
namespace EScript{
// ----------------------------------------------------

typedef uint8_t internalTypeId_t;

struct _TypeIds{
	static const internalTypeId_t TYPE_UNKNOWN 			= 0xff;

	// call by value types <0x10
	static const internalTypeId_t TYPE_NUMBER 			= 0x01;
	static const internalTypeId_t TYPE_STRING 			= 0x02;
	static const internalTypeId_t TYPE_BOOL 			= 0x03;

	// 0x20 >= AST <0x30
	static const internalTypeId_t TYPE_BLOCK_STATEMENT			= 0x20;
	static const internalTypeId_t TYPE_CONDITIONAL_EXPRESSION	= 0x21;
	static const internalTypeId_t TYPE_FOREACH_STATEMENT	 	= 0x22;
	static const internalTypeId_t TYPE_FUNCTION_CALL_EXPRESSION	= 0x23;
	static const internalTypeId_t TYPE_GET_ATTRIBUTE_EXPRESSION	= 0x24;
	static const internalTypeId_t TYPE_IF_STATEMENT				= 0x25;
	static const internalTypeId_t TYPE_LOGIC_OP_EXPRESSION		= 0x26;
	static const internalTypeId_t TYPE_LOOP_STATEMENT			= 0x27;
	static const internalTypeId_t TYPE_SET_ATTRIBUTE_EXPRESSION	= 0x28;
	static const internalTypeId_t TYPE_STATEMENT				= 0x29;
	static const internalTypeId_t TYPE_TRY_CATCH_STATEMENT		= 0x2a;
	static const internalTypeId_t TYPE_USER_FUNCTION_EXPRESSION	= 0x2b;

	// 0x30 >= various objects
	static const internalTypeId_t TYPE_FUNCTION 		= 0x30;
	static const internalTypeId_t TYPE_USER_FUNCTION 	= 0x31;
	static const internalTypeId_t TYPE_DELEGATE 		= 0x32;
	static const internalTypeId_t TYPE_ARRAY 			= 0x33;
	static const internalTypeId_t TYPE_VOID 			= 0x34;
	static const internalTypeId_t TYPE_IDENTIFIER 		= 0x35;


};


}
#endif // ES_TYPEIDS_H_INCLUDED
