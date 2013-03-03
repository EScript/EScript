// Array.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Array.h"

#include "../../Basics.h"
#include "../../StdObjects.h"
#include "../../Utils/StdConversions.h"
#include "../../Consts.h"

#include <iterator>
#include <sstream>
#include <string>
#include <stack>
#include <iostream>
#include <random>
namespace EScript{
	
//! (static)
Type * Array::getTypeObject(){
	static Type * typeObject = new Type(Collection::getTypeObject()); // ---|> Collection
	return typeObject;
}

//! initMembers
void Array::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Array new Array([Obj*]);*/
	ES_CTOR(typeObject,0,-1,Array::create(parameter))

	//! [ESMF] Array+=Obj;
	ES_MFUN(typeObject,Array,"+=",1,1,(thisObj->pushBack(parameter[0]),thisEObj))

	//!	[ESMF] thisObj Array.append(Collection);
	ES_MFUN(typeObject,Array,"append",1,1,
			(thisObj->append( parameter[0].to<Collection*>(rt) ),thisEObj))

	//! [ESMF] Object Array.back();
	ES_MFUN(typeObject,Array,"back",0,0,thisObj->back())

	//! [ESMF] thisObj Array.filter(function [,additionalParameters*])
	ES_MFUNCTION(typeObject,Array,"filter",1,-1,{
		ParameterValues additionalValues(parameter.count()-1);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+1,parameter.end(),additionalValues.begin());
		thisObj->rt_filter(rt,parameter[0],additionalValues);
		return thisObj;
	})

	//! [ESMF] Object Array.front();
	ES_MFUN(typeObject,Array,"front",0,0,thisObj->front())

	/*! [ESMF] String Array.implode([delimiter])
			\param delemiter default is ','	*/
	ES_MFUN(typeObject,Array,"implode",0,1,thisObj->implode(parameter[0].toString()))

	//! [ESMF] int|false Array.indexOf(Object[,begin])
	ES_MFUNCTION(typeObject, Array,"indexOf",1,2,{
		int i = thisObj->rt_indexOf(rt,parameter[0],parameter[1].toUInt(0));
		if(i<0)
			return false;
		else
			return i;
	})

	//! [ESMF] Obj Array.popBack();
	ES_MFUNCTION(typeObject,Array,"popBack",0,0,{
		if(thisObj->count()==0) return nullptr;
		ObjRef oRef = thisObj->back();
		thisObj->popBack();
		return oRef;
	})

	//! [ESMF] Obj Array.popFront();
	ES_MFUNCTION(typeObject, Array, "popFront", 0,0, {
		if(thisObj->count()==0) return nullptr;
		ObjRef oRef = thisObj->front();
		thisObj->popFront();
		return oRef;
	})

	//! [ESMF] thisObj Array.pushBack(Obj[,Obj...]);
	ES_MFUNCTION(typeObject,Array,"pushBack",1,-1,{
		for(const auto & param : parameter) {
			thisObj->pushBack(param);
		}
		return thisObj;
	})

	//! [ESMF] thisObj Array.pushFront(Obj,[Obj...]);
	ES_MFUNCTION(typeObject,Array,"pushFront",1,-1,{
		for(const auto & param : parameter) {
			thisObj->pushFront(param);
		}
		return  thisObj;
	})

	//! [ESMF] thisObj Array.removeIndex(int index)
	ES_MFUN(typeObject,Array,"removeIndex",1,1,(thisObj->removeIndex(parameter[0].to<uint32_t>(rt)),thisEObj)) // \todo set uint32_t to size_t when it compiles on a mac

	//! [ESMF] thisObj Array.removeValue(value [,limit [,begin]] )
	ES_MFUN(typeObject,Array,"removeValue",1,3,(thisObj->rt_removeValue(rt,parameter[0],parameter[1].toInt(-1),parameter[2].toUInt(0)),thisEObj))

	//! [ESMF] thisObj Array.resize(Number[, Object fillValue] )
	ES_MFUNCTION(typeObject,Array,"resize",1,2,{
		const size_t oldSize = thisObj->size();
		const size_t newSize = static_cast<size_t>(parameter[0].to<uint32_t>(rt)); // \todo set uint32_t to size_t when it compiles on a mac
		thisObj->resize(newSize);
		if(parameter.count()>1){
			for(size_t i = oldSize;i<newSize;++i){
				thisObj->at(i) = parameter[1]->getRefOrCopy();
			}
		}
		return thisEObj;
	})

	//! [ESMF] thisObj Array.reverse()
	ES_MFUN(typeObject,Array,"reverse",0,0,(thisObj->reverse(),thisEObj))

	/*! [ESMF] thisObj Array.rSort( [comparementFunction]);
		Like Array.sort, but the array is sorted in reverse order. */
	ES_MFUN(typeObject,Array,"rSort",0,1,(thisObj->rt_sort(rt,parameter[0].get(),true),thisEObj))

	//! [ESMF] thisObj Array.sort( [comparementFunction]);
	ES_MFUN(typeObject,Array,"sort",0,1,(thisObj->rt_sort(rt,parameter[0].get(),false),thisEObj))

	//! [ESMF] thisObj Array.splice( start,length [,Array replacement] );
	ES_MFUN(typeObject,Array,"splice",2,3,(thisObj->splice(parameter[0].to<int>(rt),parameter[1].to<int>(rt),parameter.count()>2 ? assertType<Array>(rt,parameter[2]) : nullptr),thisEObj))

	//! [ESMF] Array Array.slice( start,length );
	ES_MFUN(typeObject,Array,"slice",1,2,(thisObj->slice(parameter[0].to<int>(rt),parameter[1].toInt(0))))

	//! [ESMF] thisObj Array.swap( Array other );
	ES_MFUN(typeObject,Array,"swap",1,1,(thisObj->swap(assertType<Array>(rt,parameter[0])),thisEObj))


	// todo: removeOnce removeAll -=
}

// -----------------------------------------------------------------------

//! (static)
std::stack<Array *> Array::pool;

//! (static)
Array * Array::create(Type * type){
	Array * a = nullptr;
	if( !(type==nullptr || type==Array::getTypeObject()) || pool.empty()){
		a = new Array;
	}else{
		a = pool.top();
		pool.pop();
	}
	return a;
}

//! (static)
Array * Array::create(const ParameterValues & p,Type * type){
	Array * a = create(type);
	a->init(p);
	return a;
}

//! (static)
Array * Array::create(size_t num,Object* const* objs,Type * type){
	Array * a = create(type);
	a->init(num,objs);
	return a;
}

//! (static)
Array * Array::create(size_t num,char ** strings,Type * type){
	Array * a = create(type);
	a->init(num,strings);
	return a;
}

//! (static)
void Array::release(Array * a){
	#ifdef ES_DEBUG_MEMORY
	delete a;
	return;
	#endif
	if(pool.size()<100 && a->getType()==Array::getTypeObject()){
		a->clear();
		pool.push(a);
	}else{
		delete a;
	}
//	static size_t max = 0;
//	if(pool.size()>max){
//		std::cout << "\r"<<max;
//		max = pool.size();
//	}
}

// -----------------------------------------------------------------------

//! (internal)
void Array::init(const ParameterValues & p) {
	data.clear();
	data.reserve(p.count());
	for(size_t i = 0; i < p.count(); ++i) {
		data.emplace_back(p[i]);
	}
//	data.assign(std::begin(p), std::end(p));
}

//! (internal)
void Array::init(size_t num, Object* const* objs) {
	data.assign(objs, objs + num);
}
//! (internal)
void Array::init(size_t num, char ** strings) {
	data.clear();
	data.reserve(num);
	for(size_t i = 0; i < num; ++i) {
		data.emplace_back(EScript::create(std::string(strings[i])));
	}
}

//! ---|> [Object]
Object * Array::clone()const {
	Array * newArray = new Array(getType());

	newArray->resize(count());
	size_t i = 0;
	for(const auto & element : data) {
		newArray->data[i++] = element->getRefOrCopy();
	}
	return newArray;
}

//! ---|> Collection
Object * Array::getValue(ObjPtr key) {
	if(key.isNull()) return nullptr;
	size_t  index = static_cast<size_t>(key->toInt());
	if(index>=data.size()) return nullptr;
	return data.at(index).get();
}

//! ---|> Collection
void Array::setValue(ObjPtr key,ObjPtr value) {
	if(key.isNull() ) return;
	size_t index = static_cast<size_t>(key->toInt());
	if(index>=data.size())
		data.resize(index + 1, nullptr);
	data[index]=value;
}

//! ---|> Collection
size_t Array::count()const {
	return data.size();
}

//! ---|> Collection
Array::ArrayIterator * Array::getIterator() {
	return new ArrayIterator(this);
}

//! ---|> Collection
void Array::clear(){
	data.clear();
}

int Array::rt_indexOf(Runtime & runtime,ObjPtr search,size_t index){
	if(index>=count()||search.isNull()) return -1;

	const_iterator it = begin();
	std::advance(it, index);
	for(; it != end(); ++it) {
		if( search->isEqual(runtime,*it) )
			return index;
		++index;
	}
	return -1;
}

size_t Array::rt_removeValue(Runtime & runtime,const ObjPtr value,const int limit,const size_t start){
	if(start > size() || value.isNull() || limit==0)
		return 0;

	size_t numberOfDeletions = 0;
	std::vector<ObjRef> tempArray;
	std::vector<ObjRef>::const_iterator startIt = begin();
	std::advance(startIt, start);
	for(const_iterator it = begin();it!=end();++it) {
		if( it>=startIt && (limit<0 || numberOfDeletions<static_cast<size_t>(limit)) && value->isEqual(runtime,*it) ){
			++numberOfDeletions;
		}else{
			tempArray.push_back(*it);
		}
	}
	data.swap(tempArray);
	return numberOfDeletions;
}

void Array::removeIndex(size_t index){
	 if(index>=size())
		return;
	iterator it = data.begin();
	std::advance(it, index);
	data.erase(it);
	return ;
}

void Array::reverse(){
	if(size()>1){
		const size_t hSize = static_cast<size_t>(size()/2.0);
		size_t j = size()-1;
		for(size_t i = 0;i<hSize;++i,--j){
			ObjRef o = data[i];
			data[i]=data[j];
			data[j]=o;
		}
	}
}

std::string Array::implode(const std::string & delimiter/*=";"*/){
	std::ostringstream sprinter;

	ERef<Iterator> iRef = getIterator();
	int j = 0;
	while(! iRef->end()) {
		ObjRef value = iRef->value();
		if(!value.isNull()) {
			if(j++>0)
				sprinter << delimiter;
			sprinter << value.toString();
		}
		iRef->next();
	}
	return sprinter.str();
}

static bool compare(Runtime & runtime,Object * function,Object * a,Object * b){
	if(function!=nullptr) { // comparement function given?
		return callFunction(runtime,function,ParameterValues(a,b)).toBool();
	}else{
		return callMemberFunction(runtime,a,Consts::IDENTIFIER_fn_less,ParameterValues(b)).toBool();
	}
}


//! (implements quicksort)
void Array::rt_sort(Runtime & runtime,Object * function/*=nullptr*/,bool reverseOrder ) {
	if(count()<=1) return;

	//quicksort(runtime,0,count()-1);
	std::stack<std::pair<size_t,size_t> > pos;
	pos.push(std::make_pair(0,count()-1));

//	int cCount = 0;

	std::default_random_engine engine;
	while(! pos.empty()) {
		const size_t left = pos.top().first;
		const size_t right = pos.top().second;
		pos.pop();

		std::uniform_int_distribution<size_t> dis(left, right);

		// PARTITION
		data[left].swap(data[dis(engine)]); // permutate the first element to pick a random pivot

		/* For larger arrays select the median of three random elements.
			For good inputs, this introduces an additional overhead; for bad inputs this
			can speed up the sorting by a factor of 3 (measured). */
		if( left+100<right ){
			const size_t center = (right+left)/2;
			data[right].swap(data[dis(engine)]);
			data[center].swap(data[dis(engine)]);

			Object * s1 = data[left].get();
			Object * s2 = data[center].get();
			Object * s3 = data[right].get();

			// s1 < s2 < s3 || s1 > s2 > s3
			const bool s1_lt_s2 = compare(runtime,function,s1,s2);
//			++cCount;
			if(!runtime.checkNormalState())	return;
			const bool s2_lt_s3 = compare(runtime,function,s2,s3);
//			++cCount;
			if(!runtime.checkNormalState())	return;
			if( (s1_lt_s2&&s2_lt_s3) || (!s1_lt_s2 && !s2_lt_s3) ){
				data[left].swap(data[center]);
			}else{
				// s1 < s3 < s2 || s1 > s3 > s2
				const bool s1_lt_s3 = compare(runtime,function,s1,s3);
// 				++cCount;
				if(!runtime.checkNormalState())	return;
				if( (s1_lt_s3&&!s2_lt_s3) || (!s1_lt_s3 && s2_lt_s3) ){
					data[left].swap(data[right]);
//				}else{ // s2 < s1 < s3 || s2 > s1 > s3
				}
			}
		}
		size_t split = left;
		for(size_t i = left;i<right;++i) {
			Object * di = data[i].get();
			Object * dr = data[right].get();

			const bool change = compare(runtime,function,di,dr);
//			++cCount;
			if(!runtime.checkNormalState())
				return;

			if(change^reverseOrder) {
				data[i].swap(data[split]);
				++split;
			}
		}
		data[split].swap(data[right]);

		//--
		//quicksort(runtime,left,split-1);
		if(split > left+1) // left==split -> no elements to sort, left+1==split -> only one element
			pos.push(std::make_pair(left,split-1));

		//quicksort(runtime,split+1,right);
		if(right > split+1) // right==split -> no elements to sort, right==split+1 -> only one element
			pos.push(std::make_pair(split+1,right));
	}
//	std::cout << " ("<<cCount<<")";
}


void Array::rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues) {
	std::vector<ObjRef> tempArray;

	ParameterValues parameters(additionalValues.count()+1);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+1);

	for(const auto & element : data) {
		parameters.set(0, element);
		if( callFunction(runtime,function.get(),parameters).toBool() ){
			tempArray.push_back(element);
		}
	}
	data.swap(tempArray);
}

void Array::append(Collection * c){
	if(c==nullptr || c==this)
		return;
	size_t i = count();
	resize( count()+c->count() );
	for(ERef<Iterator> iRef = c->getIterator(); !iRef->end() ;iRef->next()){
		ObjRef value = iRef->value();
		if(!value.isNull())
			data[i++]=value->getRefOrCopy();
	}
}

void Array::swap(Array * other){
	data.swap(other->data);
}

void Array::resize(size_t newSize){
	data.resize(newSize);
}

void Array::reserve(size_t capacity){
	data.reserve(capacity);
}

void Array::splice(int startIndex,int length,Array * replacement){
	if(startIndex<0){
		startIndex = std::max( static_cast<int>(data.size())+startIndex , 0);
	}
	if(length<0){
		length = std::max( (static_cast<int>(data.size())+length) - startIndex ,0);
	}

	// at the end?
	if(startIndex>=static_cast<int>(data.size())){
		if(replacement!=nullptr)
			append(replacement);
		return;
	}

	container_t tmp;
	for(size_t i = 0;i<static_cast<size_t>(startIndex);++i){
		tmp.push_back(data[i]);
	}
	if(replacement!=nullptr){
		for(const auto & element : replacement->data) {
			tmp.push_back(element->getRefOrCopy());
		}
	}

	for(size_t i = startIndex+length; i<data.size(); ++i ){
		tmp.push_back(data[i]);
	}
	data.swap(tmp);
}


Array * Array::slice(int startIndex,int length){
	ERef<Array> result = Array::create();
	if(startIndex<0)
		startIndex = std::max( static_cast<int>(data.size())+startIndex, 0);
	if(static_cast<size_t>(startIndex)<data.size()){
		size_t endIndex = length>0 ? startIndex+length : data.size()+length;

		for(size_t i = static_cast<size_t>(startIndex); i<endIndex; ++i)
			result->pushBack( data[i]->getRefOrCopy() );
	}
	return result.detachAndDecrease();
}

// ------- ArrayIterator

//! (ctor) Array::ArrayIterator
Array::ArrayIterator::ArrayIterator(Array * ar):Iterator(),arrayRef(ar),index(0) {
}

//! [dtor] Array::ArrayIterator
Array::ArrayIterator::~ArrayIterator() {
}

//! ---|> [Iterator]
Object * Array::ArrayIterator::key() {
	return end() ? nullptr : EScript::create(static_cast<uint32_t>(index));
}

//! ---|> [Iterator]
Object * Array::ArrayIterator::value() {
	return end() ? nullptr : arrayRef->data[index].get();
}

//! ---|> [Iterator]
void Array::ArrayIterator::next() {
	++index;
}

//! ---|> [Iterator]
void Array::ArrayIterator::reset() {
	index = 0;
}

//! ---|> [Iterator]
bool Array::ArrayIterator::end() {
	return index>=arrayRef->data.size();
}

//template<>
//Array* convertTo<Array*>(Runtime& runtime,ObjPtr src)		{	return assertType<Array>(rt,src);	}


}//namespace EScript
