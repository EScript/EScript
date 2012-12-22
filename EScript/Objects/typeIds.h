// typeIds.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_TYPEIDS_H_INCLUDED
#define ES_TYPEIDS_H_INCLUDED

#include <cstdint>
namespace EScript{
// ----------------------------------------------------

typedef uint8_t internalTypeId_t;

struct _TypeIds{
	static const internalTypeId_t TYPE_UNKNOWN			= 0xff;

	// call by value types <0x10
	static const internalTypeId_t TYPE_NUMBER			= 0x01;
	static const internalTypeId_t TYPE_STRING			= 0x02;
	static const internalTypeId_t TYPE_BOOL				= 0x03;

	// 0x10 >= mixed
	static const internalTypeId_t TYPE_FUNCTION			= 0x10;
	static const internalTypeId_t TYPE_USER_FUNCTION	= 0x11;
	static const internalTypeId_t TYPE_DELEGATE			= 0x12;
	static const internalTypeId_t TYPE_ARRAY			= 0x13;
	static const internalTypeId_t TYPE_VOID				= 0x14;
	static const internalTypeId_t TYPE_IDENTIFIER		= 0x15;
	static const internalTypeId_t TYPE_TYPE				= 0x16;
};


}
#endif // ES_TYPEIDS_H_INCLUDED
