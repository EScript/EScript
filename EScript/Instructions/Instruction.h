// Instruction.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../Utils/StringId.h"
#include <string>
#include <cstddef>

namespace EScript {

class InstructionBlock;

/*! [Instruction]
	Work in progress!	*/
class Instruction {
	public:
		enum type_t{
			I_ASSIGN_ATTRIBUTE,				// -2
			I_ASSIGN_LOCAL,					// -1
			I_ASSIGN_VARIABLE,				// -1
			I_CALL,							// -2+x +1
			I_CREATE_INSTANCE,				// -1+x +1
			I_CHECK_TYPE,					// -1   +1
			I_DUP,							// +1
			I_FIND_VARIABLE,				// +2
			I_GET_ATTRIBUTE,				// -1 +1
			I_GET_VARIABLE,					// +1
			I_GET_LOCAL_VARIABLE,			// +1
			I_INIT_CALLER,					// -x +0
			I_JMP,							// +-0
			I_JMP_IF_SET,					// -1
			I_JMP_ON_TRUE,					// -1
			I_JMP_ON_FALSE,					// -1
			I_NOT,							// -1 +1
			I_POP,							// -1
			I_PUSH_BOOL,					// +1
			I_PUSH_ID,						// +1
			I_PUSH_FUNCTION,				// +1
			I_PUSH_NUMBER,					// +1
			I_PUSH_STRING,					// +1
			I_PUSH_UINT,					// +1
			I_PUSH_UNDEFINED,				// +1
			I_PUSH_VOID,					// +1
			I_RESET_LOCAL_VARIABLE,			// -x
			I_SET_ATTRIBUTE,				// -3
			I_SET_EXCEPTION_HANDLER,		// +-0
			I_SYS_CALL,						// +-?
			I_YIELD,						// -1
			I_UNDEFINED,
			I_SET_MARKER					// +-0
		};
		static const uint32_t JMP_TO_MARKER_OFFSET = 0x100000; //! if a jump target is >= JMP_TO_MARKER_OFFSET, the target is a marker and not an address.
		static const uint32_t INVALID_JUMP_ADDRESS = 0x0FFFFF; //! A jump to this address always ends the current function. \todo assure that no IntructionBlock can have so many Instructions

		std::string toString(const InstructionBlock & ctxt)const;

		type_t getType()const						{	return type;	}

		uint32_t getValue_uint32()const				{	return data.value_uint32;	}
		void setValue_uint32(const uint32_t v)		{	data.value_uint32 = v;	}

		double getValue_Number()const				{	return data.value_number;	}
		void setValue_Number(double v)				{	data.value_number = v;	}

		StringId getValue_Identifier()const			{	return StringId(data.value_identifier);	}
		void setValue_Identifier(StringId v)		{	data.value_identifier = v.getValue();	}

		bool getValue_Bool()const					{	return data.value_bool;	}
		void setValue_Bool(bool v)					{	data.value_bool = v;	}
		
		std::pair<uint32_t,uint32_t> getValue_uint32Pair()const	{	return data.value_uint32Pair;	}
		void setValue_uint32Pair(uint32_t v1,uint32_t v2)	{	data.value_uint32Pair = std::make_pair(v1,v2);	}

		static Instruction createAssignAttribute(const StringId & varName);
		static Instruction createAssignLocal(const uint32_t localVarIdx);
		static Instruction createAssignVariable(const StringId & varName);
		static Instruction createCall(const uint32_t numParams);
		static Instruction createCheckType(const uint32_t localVarIdx);
		static Instruction createCreateInstance(const uint32_t numParams);
		static Instruction createDup()				{	return Instruction(I_DUP);	}
		static Instruction createFindVariable(const StringId & id);
		static Instruction createGetAttribute(const StringId & id);
		static Instruction createGetLocalVariable(const uint32_t localVarIdx);
		static Instruction createGetVariable(const StringId & id);
		static Instruction createInitCaller(const uint32_t numSuperParams);
		static Instruction createJmp(const uint32_t markerId);
		static Instruction createJmpIfSet(const uint32_t markerId);
		static Instruction createJmpOnTrue(const uint32_t markerId);
		static Instruction createJmpOnFalse(const uint32_t markerId);
		static Instruction createNot()				{	return Instruction(I_NOT);	}
		static Instruction createPop()				{	return Instruction(I_POP);	}
		static Instruction createPushBool(const bool value);
		static Instruction createPushId(const StringId & id);
		static Instruction createPushFunction(const uint32_t functionIdx);
		static Instruction createPushNumber(const double value);
		static Instruction createPushString(const uint32_t stringIndex);
		static Instruction createPushUInt(const uint32_t value);
		static Instruction createPushUndefined()	{	return Instruction(I_PUSH_UNDEFINED);	}
		static Instruction createPushVoid()			{	return Instruction(I_PUSH_VOID);	}
		static Instruction createResetLocalVariable(const uint32_t localVarIdx);
		static Instruction createSetAttribute(const StringId & id);
		static Instruction createSetExceptionHandler(const uint32_t markerId);
		static Instruction createSetMarker(const uint32_t markerId);
		static Instruction createSysCall(const uint32_t fnIdx, const uint32_t numParams);
		static Instruction createYield()			{	return Instruction(I_YIELD);	}

		int getLine()const			{	return line;	}
		void setLine(const int l)	{	line = l;	}

	private:
		Instruction( type_t _type) : type(_type),line(-1){}
		type_t type;
		union value_t{
			double value_number;
			size_t value_numParams;
			uint32_t value_identifier;
			uint32_t value_uint32;
			bool value_bool;
			std::pair<uint32_t,uint32_t> value_uint32Pair;
			uint64_t raw;
			value_t():raw(0){static_assert(sizeof(raw)==sizeof(value_t),"'raw' must cover the whole union.");}
			value_t(value_t && other) : raw(other.raw) {}
			value_t(const value_t & other) : raw(other.raw) {}
			value_t & operator=(value_t && other) {
				raw = other.raw;
				return *this;
			}
			value_t & operator=(const value_t & other) {
				raw = other.raw;
				return *this;
			}
		}data;
		int line;
};
}

#endif // INSTRUCTION_H
