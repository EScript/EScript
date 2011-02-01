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

	// 0x20 >= expressions <0x30
	static const internalTypeId_t TYPE_GET_ATTRIBUTE 	= 0x20;
	static const internalTypeId_t TYPE_FUNCTION_CALL 	= 0x21;
	static const internalTypeId_t TYPE_SET_ATTRIBUTE 	= 0x22;
	static const internalTypeId_t TYPE_LOGIC_OP 		= 0x23;
	static const internalTypeId_t TYPE_CONDITIONAL 		= 0x24;
	static const internalTypeId_t TYPE_BLOCK 			= 0x25;

	// 0x30 >= various objects
	static const internalTypeId_t TYPE_FUNCTION 		= 0x30;
	static const internalTypeId_t TYPE_USER_FUNCTION 	= 0x31;
	static const internalTypeId_t TYPE_DELEGATE 		= 0x32;
	static const internalTypeId_t TYPE_ARRAY 			= 0x33;


};


}
#endif // ES_TYPEIDS_H_INCLUDED
