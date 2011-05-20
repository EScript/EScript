// StringData.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef STRINGDATA_H
#define STRINGDATA_H

#include <string>
#include <stack>

namespace EScript {

/*! [StringData]	*/
class StringData{

	//! internals
		struct Data{
			Data(const std::string & _s) : s(_s),referenceCounter(0){}
			std::string s;
			int referenceCounter;
		};
		static std::stack<StringData::Data*> dataPool;
		static Data * createData(const std::string & s);
		static void releaseData(Data * data);

		void setData(Data * newData);
		Data * data;
		static Data * getEmptyData();
	public:
		StringData() : data(getEmptyData())								{	++data->referenceCounter;	} 
		explicit StringData(const std::string & s) : data(createData(s)){	++data->referenceCounter;	}
		StringData(const StringData & other) : data(other.data)			{	++data->referenceCounter;	}
		
		~StringData(){
			if( (--data->referenceCounter) <=0 )
				releaseData(data);
		}

		bool empty()const								{	return str().empty();	}
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
