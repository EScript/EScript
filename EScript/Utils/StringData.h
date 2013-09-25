// StringData.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRINGDATA_H
#define STRINGDATA_H

#include <string>
#include <memory>
#include <vector>
#include <stack>

namespace EScript {

//! [StringData]
class StringData{

	//! internals
		struct Data{
			std::string s;
			int referenceCounter;
			enum dataType_t{
				RAW,					// the string consists of bytes without special semantic
				ASCII,					// the string consists only of ascii-characters (<128)
				UNKNOWN_UNICODE,		// the string contains of an unknown number of unknown code points
				UNICODE_WITH_LENGTH,	// the string contains of a known number of code points
				UNICODE_WITH_JUMTABLE	// the string contains of a known number of code points and contains
										//  a jump table for random accesses
			} dataType;
			std::unique_ptr<std::vector<size_t>> jumpTable; //!< jumpTable[i] := strPos of codePoint( (i+1)*JUMP_TABLE_STEP_SIZE)
			size_t numCodePoints;

			Data(const std::string & _s,dataType_t t) : 
				s(_s),referenceCounter(0),dataType(t),numCodePoints(0){}
			Data(const char * c,size_t size,dataType_t t) : 
				s(c,size),referenceCounter(0),dataType(t),numCodePoints(0){}
			Data(Data &&) = default;
			Data(const Data &) = delete;
			void initJumpTable();

		};
		static Data * createData(const std::string & s);
		static Data * createData(const char * c,size_t size);
		static void releaseData(Data * data);

		void setData(Data * newData);
		Data * data;
		static Data * getEmptyData();
		static std::stack<Data*> dataPool;
		
		void initJumpTable()const;
	public:
		StringData() : data(getEmptyData())								{	++data->referenceCounter;	}
		explicit StringData(const std::string & s) : data(createData(s)){	++data->referenceCounter;	}
		explicit StringData(const char * c,size_t size) : data(createData(c,size)){	++data->referenceCounter;	}
		StringData(const StringData & other) : data(other.data)			{	++data->referenceCounter;	}

		~StringData(){
			if( (--data->referenceCounter) <=0 )
				releaseData(data);
		}
		/*! Returns the byte index of the given codePointIdx in the utf8 encoded string.
			If the codePoint is invalid, std::string::npos is returned. */
		size_t codePointToBytePos(const size_t codePointNr)const;
		bool empty()const								{	return str().empty();	}
		
		size_t getDataSize()const						{	return str().length();	}
		size_t getNumCodepoints()const;
		std::string getSubStr(const size_t codePointStart, const size_t numCodePoints)const;
		
		size_t find(const std::string& subj,const size_t codePointStart=0)const;
		size_t rFind(const std::string& subj,const size_t codePointStart=std::string::npos)const;

		bool operator==(const StringData & other)const	{	return (data==other.data) || (str()==other.str()); }
		StringData & operator=(const StringData & other){
			setData(other.data);
			return *this;
		}
		StringData & operator=(const std::string & s){
			setData(createData(s));
			return *this;
		}
		void set(const StringData & other)				{	setData(other.data);	}
		void set(const std::string & s)					{	setData(createData(s));	}
		const std::string & str()const					{	return data->s;	}
};


}
#endif // STRINGDATA_H
