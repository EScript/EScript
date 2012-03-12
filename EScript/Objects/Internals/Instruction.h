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
			I_CALL,
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
			I_PUSH_VOID,
			I_SET_MARKER,
		};

		std::string toString()const;

		type_t getType()const						{	return type;	}

		double getValue_Number()const				{	return value_number;	}
		void setValue_Number(double v)				{	value_number=v;	}

		size_t getValue_NumParams()const			{	return value_numParams;	}
		void setValue_NumParams(size_t v)			{	value_numParams=v;	}

		StringId getValue_Identifier()const			{	return StringId(value_identifier);	}
		void setValue_Identifier(StringId v)		{	value_identifier=v.getValue();	}

		StringId getValue_MarkerId()const			{	return StringId(value_markerId);	}
		void setValue_MarkerId(StringId v)			{	value_markerId=v.getValue();	}

		bool getValue_Bool()const					{	return value_bool;	}
		void setValue_Bool(bool v)					{	value_bool=v;	}

		const std::string & getValue_String()const	{	return value_str;	}
		void setValue_String(const std::string & v)	{	value_str=v;	}

		static Instruction createCall(const Statement & _stmt,const size_t numParams);
		static Instruction createFindVariable(const Statement & _stmt,const StringId id);
		static Instruction createGetAttribute(const Statement & _stmt,const StringId id);
		static Instruction createGetVariable(const Statement & _stmt,const StringId id);
		static Instruction createJmp(const Statement & _stmt,const StringId id);
		static Instruction createJmpOnTrue(const Statement & _stmt,const StringId id);
		static Instruction createJmpOnFalse(const Statement & _stmt,const StringId id);
		static Instruction createNot(const Statement & _stmt);
		static Instruction createPop(const Statement & _stmt);
		static Instruction createPushBool(const Statement & _stmt,const bool value);
		static Instruction createPushId(const Statement & _stmt,const StringId id);
		static Instruction createPushNumber(const Statement & _stmt,const double value);
		static Instruction createPushString(const Statement & _stmt,const std::string & value);
		static Instruction createPushVoid(const Statement & _stmt);
		static Instruction createSetMarker(const Statement & _stmt,const StringId id);

	private:
		Instruction( type_t _type, const Statement & _stmt) : type(_type){}//,stmt(_stmt){}
		

		type_t type;
		union{
			double value_number;
			size_t value_numParams;
			uint32_t value_identifier;
			uint32_t value_markerId;
			bool value_bool;
		};
		std::string value_str; // to be removed!!!!!!!
//		Statement stmt; // to be removed!!!!!!!
};
}

#endif // INSTRUCTION_H
