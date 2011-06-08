// StringData.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "StringData.h"

namespace EScript{

// (internals)

std::stack<StringData::Data*> StringData::dataPool;

//! (static,internal)
StringData::Data * StringData::createData(const std::string & s){
	if(s.empty())
		return getEmptyData();
	else if(dataPool.empty()){
		return new Data(s);
	}else{
		Data * d=dataPool.top();
		dataPool.pop();
		d->s=s;
		return d;
	}
}

//! (static,internal)
void StringData::releaseData(Data * data){
	data->s.clear();
	dataPool.push(data);
}

//! (internal)
void StringData::setData(Data * newData){
	if(newData!=data){
		if(--data->referenceCounter <=0 )
			releaseData(data);
		++newData->referenceCounter;
		data=newData;
	}
}

//! (static,internal)
StringData::Data * StringData::getEmptyData(){
	struct UndeletableEmptyStringFactory{
		static Data * create(){
			Data * emptyString=new Data("");
			++emptyString->referenceCounter;
			return emptyString;
		}
	};
	static Data * emptyString=UndeletableEmptyStringFactory::create();
	return emptyString;
}

}
