// Array.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Array.h"

#include "../../EScript.h"

#include <iterator>
#include <sstream>
#include <string>
#include <stack>
#include <iostream>
#include <random>
namespace EScript{

using std::vector;

//! (static)
Type * Array::getTypeObject(){
	// [Array] ---|> [Collection]
	static Type * typeObject=new Type(Collection::getTypeObject());
	return typeObject;
}

//! initMembers
void Array::init(EScript::Namespace & globals) {
	Type * typeObject = getTypeObject();
	initPrintableName(typeObject,getClassName());

	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Array new Array([Obj*]);*/
	ESF_DECLARE(typeObject, "_constructor", 0,-1,
		Array::create(parameter))

	//! [ESMF] Array+=Obj;
	ESMF_DECLARE(typeObject,Array,"+=",1,1,(self->pushBack(parameter[0]),self))

	//!	[ESMF] self Array.append(Collection);
	ESMF_DECLARE(typeObject,Array,"append",1,1,
			(self->append(assertType<Collection>(runtime,parameter[0])),self))

	//! [ESMF] Object Array.back();
	ESMF_DECLARE(typeObject,Array,"back",0,0,self->back())

	//! [ESMF] self Array.filter(function [,additionalParameters*])
	ES_MFUNCTION_DECLARE(typeObject,Array,"filter",1,-1,{
		ParameterValues additionalValues(parameter.count()-1);
		if(!additionalValues.empty())
			std::copy(parameter.begin()+1,parameter.end(),additionalValues.begin());
		self->rt_filter(runtime,parameter[0],additionalValues);
		return self;
	})

	//! [ESMF] Object Array.front();
	ESMF_DECLARE(typeObject,Array,"front",0,0,self->front())

	/*! [ESMF] String Array.implode([delimiter])
			\param delemiter default is ','	*/
	ESMF_DECLARE(typeObject,Array,"implode",0,1,String::create(self->implode(parameter[0].toString())))

	//! [ESMF] int|false Array.indexOf(Object[,begin])
	ES_MFUNCTION_DECLARE(typeObject, Array,"indexOf",1,2,{
		int i=self->rt_indexOf(runtime,parameter[0],parameter[1].toUInt(0));
		if(i<0)
			return Bool::create(false);
		else
			return Number::create(i);
	})

	//! [ESMF] Obj Array.popBack();
	ES_MFUNCTION_DECLARE(typeObject,Array,"popBack",0,0,{
		if (self->count()==0) return nullptr;
		ObjRef oRef=self->back();
		self->popBack();
		return oRef.detachAndDecrease();
	})

	//! [ESMF] Obj Array.popFront();
	ES_MFUNCTION_DECLARE(typeObject, Array, "popFront", 0,0, {
		if (self->count()==0) return nullptr;
		ObjRef oRef=self->front();
		self->popFront();
		return oRef.detachAndDecrease();
	})

	//! [ESMF] self Array.pushBack(Obj[,Obj...]);
	ES_MFUNCTION_DECLARE(typeObject,Array,"pushBack",1,-1,{
		for(const auto & param : parameter) {
			self->pushBack(param);
		}
		return  self;
	})

	//! [ESMF] self Array.pushFront(Obj,[Obj...]);
	ES_MFUNCTION_DECLARE(typeObject,Array,"pushFront",1,-1,{
		for(const auto & param : parameter) {
			self->pushFront(param);
		}
		return  self;
	})

	//! [ESMF] self Array.removeIndex(int index)
	ESMF_DECLARE(typeObject,Array,"removeIndex",1,1,(self->removeIndex(parameter[0].toUInt()),self))

	//! [ESMF] self Array.removeValue(value [,limit [,begin]] )
	ESMF_DECLARE(typeObject,Array,"removeValue",1,3,(self->rt_removeValue(runtime,parameter[0],parameter[1].toInt(-1),parameter[2].toUInt(0)),self))

	//! [ESMF] self Array.resize(Number[, Object fillValue] )
	ES_MFUNCTION_DECLARE(typeObject,Array,"resize",1,2,{
		const size_t oldSize = self->size();
		const size_t newSize = static_cast<size_t>(parameter[0].toUInt());
		self->resize(newSize);
		if(parameter.count()>1){
			for(size_t i=oldSize;i<newSize;++i){
				self->at(i) = parameter[1]->getRefOrCopy();
			}
		}
		return self;
	})

	//! [ESMF] self Array.reverse()
	ESMF_DECLARE(typeObject,Array,"reverse",0,0,(self->reverse(),self))

	/*! [ESMF] self Array.rSort( [comparementFunction]);
		Like Array.sort, but the array is sorted in reverse order. */
	ESMF_DECLARE(typeObject,Array,"rSort",0,1,(self->rt_sort(runtime,parameter[0].get(),true),self))

	//! [ESMF] self Array.sort( [comparementFunction]);
	ESMF_DECLARE(typeObject,Array,"sort",0,1,(self->rt_sort(runtime,parameter[0].get(),false),self))

	//! [ESMF] self Array.splice( start,length [,Array replacement] );
	ESMF_DECLARE(typeObject,Array,"splice",2,3,(self->splice(parameter[0].toInt(),parameter[1].toInt(),parameter.count()>2 ? assertType<Array>(runtime,parameter[2]) : nullptr),self))

	//! [ESMF] Array Array.slice( start,length );
	ESMF_DECLARE(typeObject,Array,"slice",1,2,(self->slice(parameter[0].toInt(),parameter[1].toInt(0))))

	//! [ESMF] self Array.swap( Array other );
	ESMF_DECLARE(typeObject,Array,"swap",1,1,(self->swap(assertType<Array>(runtime,parameter[0])),self))


	// todo: removeOnce removeAll -=
}

// -----------------------------------------------------------------------

//! (static)
std::stack<Array *> Array::pool;

//! (static)
Array * Array::create(Type * type){
	Array * a=nullptr;
	if( !(type==nullptr || type==Array::getTypeObject()) || pool.empty()){
		a=new Array();
	}else{
		a=pool.top();
		pool.pop();
	}
	return a;
}

//! (static)
Array * Array::create(const ParameterValues & p,Type * type){
	Array * a=create(type);
	a->init(p);
	return a;
}

//! (static)
Array * Array::create(size_t num,Object ** objs,Type * type){
	Array * a=create(type);
	a->init(num,objs);
	return a;
}

//! (static)
Array * Array::create(size_t num,char ** strings,Type * type){
	Array * a=create(type);
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
//	static size_t max=0;
//	if(pool.size()>max){
//		std::cout << "\r"<<max;
//		max=pool.size();
//	}
}

// -----------------------------------------------------------------------
//! (ctor)
Array::Array(Type * type):Collection(type?type:getTypeObject()) {
	//ctor
}

//! (dtor)
Array::~Array() {
	//dtor
}

//! (internal)
void Array::init(const ParameterValues & p) {
	data.assign(std::begin(p), std::end(p));
}

/*!	(internal)*/
void Array::init(size_t num, Object ** objs) {
	data.assign(objs, objs + num);
}
/*!	(internal)*/
void Array::init(size_t num, char ** strings) {
	data.clear();
	data.reserve(num);
	for(size_t i = 0; i < num; ++i) {
		data.emplace_back(String::create(strings[i]));
	}
}

//! ---|> [Object]
Object * Array::clone()const {
	Array * newArray=new Array(getType());

	newArray->resize(count());
	size_t i=0;
	for(const auto & element : data) {
		newArray->data[i++] = element->getRefOrCopy();
	}
	return newArray;
}

/*!	---|> Collection*/
Object * Array::getValue(ObjPtr key) {
	if (key.isNull()) return nullptr;
	size_t  index=static_cast<size_t>(key->toInt());
	if (index>=data.size()) return nullptr;
	return data.at(index).get();
}

/*!	---|> Collection*/
void Array::setValue(ObjPtr key,ObjPtr value) {
	if (key.isNull() ) return;
	size_t index=static_cast<size_t>(key->toInt());
	if (index>=data.size())
		data.resize(index + 1, nullptr);
	data[index]=value;
}

/*!	---|> Collection*/
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

	size_t numberOfDeletions=0;
	std::vector<ObjRef> tempArray;
	std::vector<ObjRef>::const_iterator startIt = begin();
	std::advance(startIt, start);
	for (const_iterator it=begin();it!=end();++it) {
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
		const size_t hSize=static_cast<size_t>(size()/2.0);
		size_t j=size()-1;
		for(size_t i=0;i<hSize;++i,--j){
			ObjRef o=data[i];
			data[i]=data[j];
			data[j]=o;
		}
	}
}

std::string Array::implode(const std::string & delimiter/*=";"*/){
	std::ostringstream sprinter;

	ERef<Iterator> iRef=getIterator();
	int j=0;
	while (! iRef->end()) {
		ObjRef value=iRef->value();
		if (!value.isNull()) {
			if (j++>0)
				sprinter << delimiter;
			sprinter << value.toString();
		}
		iRef->next();
	}
	return sprinter.str();
}

static bool compare(Runtime & runtime,Object * function,Object * a,Object * b){
	if (function!=nullptr) { // comparement function given?
		ObjRef result=callFunction(runtime,function,ParameterValues(a,b));
		return result.toBool();
	}else{
		ObjRef result=callMemberFunction(runtime,a,Consts::IDENTIFIER_fn_less,ParameterValues(b));
		return result.toBool();
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
		const size_t left=pos.top().first;
		const size_t right=pos.top().second;
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
		size_t split=left;
		for (size_t i=left;i<right;++i) {
			Object * di=data[i].get();
			Object * dr=data[right].get();

			const bool change=compare(runtime,function,di,dr);
//			++cCount;
			if(!runtime.checkNormalState())
				return;

			if (change^reverseOrder) {
				data[i].swap(data[split]);
				split++;
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
		ObjRef resultRef=callFunction(runtime,function.get(),parameters);
		if( resultRef.toBool() ){
			tempArray.push_back(element);
		}
	}
	data.swap(tempArray);
}

void Array::append(Collection * c){
	if(c==nullptr || c==this)
		return;
	size_t i=count();
	resize( count()+c->count() );
	for(ERef<Iterator> iRef=c->getIterator(); !iRef->end() ;iRef->next()){
		ObjRef value=iRef->value();
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
	return end() ? nullptr : Number::create(index);
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
	index=0;
}

//! ---|> [Iterator]
bool Array::ArrayIterator::end() {
	return index>=arrayRef->data.size();
}
}//namespace EScript
