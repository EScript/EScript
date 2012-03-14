// Instruction.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Statement.h"

namespace EScript {

/*! [Instruction]  
	Work in progress!	*/
class Instruction {
	public:
		enum type_t{
			I_UNDEFINED,
			I_ASSIGN,
			I_ASSIGN_LOCAL,
			I_CALL,
			I_DUP,
			I_FIND_VARIABLE,
			I_GET_ATTRIBUTE,
			I_GET_VARIABLE,
			I_JMP,
			I_JMP_ON_TRUE,
			I_JMP_ON_FALSE,
			I_NOT,
			I_POP,
			I_PUSH_BOOL,
			I_PUSH_ID,
			I_PUSH_NUMBER,
			I_PUSH_STRING,
			I_PUSH_UINT,
			I_PUSH_VOID,
			I_SET_ATTRIBUTE,
			I_SET_MARKER,
		};

		std::string toString()const;

		type_t getType()const						{	return type;	}

		uint32_t getValue_uint32()const				{	return value_uint32;	}
		void setValue_uint32(const uint32_t v)		{	value_uint32 = v;	}
		
		double getValue_Number()const				{	return value_number;	}
		void setValue_Number(double v)				{	value_number=v;	}

		StringId getValue_Identifier()const			{	return StringId(value_identifier);	}
		void setValue_Identifier(StringId v)		{	value_identifier=v.getValue();	}

		bool getValue_Bool()const					{	return value_bool;	}
		void setValue_Bool(bool v)					{	value_bool=v;	}

		const std::string & getValue_String()const	{	return value_str;	}
		void setValue_String(const std::string & v)	{	value_str=v;	}

		static Instruction createAssign(const StringId varName);
		static Instruction createAssignLocal(const uint32_t localVarIdx);
		static Instruction createCall(const uint32_t numParams);
		static Instruction createDup()				{	return Instruction(I_DUP);	}
		static Instruction createFindVariable(const StringId id);
		static Instruction createGetAttribute(const StringId id);
		static Instruction createGetVariable(const StringId id);
		static Instruction createJmp(const StringId id);
		static Instruction createJmpOnTrue(const StringId id);
		static Instruction createJmpOnFalse(const StringId id);
		static Instruction createNot();
		static Instruction createPop();
		static Instruction createPushBool(const bool value);
		static Instruction createPushId(const StringId id);
		static Instruction createPushNumber(const double value);
		static Instruction createPushString(const std::string & value);
		static Instruction createPushUInt(const uint32_t value);
		static Instruction createPushVoid();
		static Instruction createSetAttribute(const StringId id);
		static Instruction createSetMarker(const StringId id);

		void setLine(int l)	{	line = l;	}

	private:
		Instruction( type_t _type) : type(_type),line(-1){}//,stmt(_stmt){}
		

		type_t type;
		union{
			double value_number;
			size_t value_numParams;
			uint32_t value_identifier;
			uint32_t value_uint32;
			bool value_bool;
		};
		std::string value_str; // to be removed!!!!!!!
		int line;
//		Statement stmt; // to be removed!!!!!!!
};
}

#endif // INSTRUCTION_H
