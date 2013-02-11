// Exception.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "ExtObject.h"

namespace EScript {

//! [Exception] ---|> [ExtObject] ---|> [Object]
class Exception : public ExtObject {
		ES_PROVIDES_TYPE_NAME(Exception)
	public:
		static Type* getTypeObject();
		static void init(EScript::Namespace & globals);
		// ----

		explicit Exception(const std::string & msg,int line = 0,Type * type = nullptr);
		virtual ~Exception()							{	}

		void setMessage(const std::string & newMessage)	{	msg = newMessage;	}
		const std::string & getMessage()const			{	return msg;	}

		int getLine()const								{	return line;	}
		void setLine(int newLine)						{	line = newLine;	}

		const std::string & getStackInfo()const			{	return stackInfo;	}
		void setStackInfo(const std::string & s)		{	stackInfo = s;	}

		void setFilename(const std::string & filename)	{	filenameId = filename;	}
		void setFilenameId(StringId _filenameId)		{	filenameId = _filenameId;	}
		std::string getFilename()const					{	return filenameId.toString();	}
		StringId getFilenameId()const					{	return filenameId;	}

		//! ---|> [Object]
		virtual Object * clone()const;
		virtual std::string toString()const;

	protected:
		std::string msg;
		std::string stackInfo;
		int line;
		StringId filenameId;
};

}

#endif // EXCEPTION_H
