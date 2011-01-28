// Array.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Array.h"

#include "../EScript.h"

#include <sstream>
#include <string>
#include <stack>

namespace EScript{

using std::vector;

Type* Array::typeObject=NULL;


//! initMembers
void Array::init(EScript::Namespace & globals) {
//
	// Array ---|> Collection
	typeObject=new Type(Collection::getTypeObject());
	declareConstant(&globals,getClassName(),typeObject);

	//! [ESMF] Array new Array([Obj*]);*/
	ESF_DECLARE(typeObject, "_constructor", 0,-1,
		Array::create(parameter,dynamic_cast<Type*>(caller)))

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
		int i=self->rt_indexOf(runtime,parameter[0],parameter[1].toInt(0));
		if(i<0)
			return Bool::create(false);
		else
			return Number::create(i);
	})

	//! [ESMF] Obj Array.popBack();
	ES_MFUNCTION_DECLARE(typeObject,Array,"popBack",0,0,{
		if (self->count()==0) return NULL;
		ObjRef oRef=self->back();
		self->popBack();
		return oRef.detachAndDecrease();
	})

	//! [ESMF] Obj Array.popFront();
	ES_MFUNCTION_DECLARE(typeObject, Array, "popFront", 0,0, {
		if (self->count()==0) return NULL;
		ObjRef oRef=self->front();
		self->popFront();
		return oRef.detachAndDecrease();
	})

	//! [ESMF] self Array.pushBack(Obj[,Obj...]);
	ES_MFUNCTION_DECLARE(typeObject,Array,"pushBack",1,-1,{
		for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it)
			self->pushBack(*it);
		return  self;
	})

	//! [ESMF] self Array.pushFront(Obj,[Obj...]);
	ES_MFUNCTION_DECLARE(typeObject,Array,"pushFront",1,-1,{
		for(ParameterValues::const_iterator it=parameter.begin();it!=parameter.end();++it)
			self->pushFront(*it);
		return  self;
	})

	//! [ESMF] Array Array.removeIndex(int index)
	ESMF_DECLARE(typeObject,Array,"removeIndex",1,1,(self->removeIndex(parameter[0]->toInt()),self))

	//! [ESMF] self Array.reverse()
	ESMF_DECLARE(typeObject,Array,"reverse",0,0,(self->reverse(),self))

	/*! [ESMF] self Array.rSort( [comparementFunction]);
		Like Array.sort, but the array is sorted in reverse order. */
	ESMF_DECLARE(typeObject,Array,"rSort",0,1,(self->rt_sort(runtime,parameter[0].get(),true),self))

	//! [ESMF] self Array.sort( [comparementFunction]);
	ESMF_DECLARE(typeObject,Array,"sort",0,1,(self->rt_sort(runtime,parameter[0].get(),false),self))

	//! [ESMF] self Array.swap( Array other );
	ESMF_DECLARE(typeObject,Array,"swap",1,1,(self->swap(assertType<Array>(runtime,parameter[0])),self))

	// todo: removeOnce removeAll -=
}

// -----------------------------------------------------------------------

//! (static)
std::stack<Array *> Array::pool;

//! (static)
Array * Array::create(Type * type){
	Array * a=NULL;
	if( !(type==NULL || type==Array::typeObject) || pool.empty()){
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
	if(pool.size()<100 && a->getType()==Array::typeObject){
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
Array::Array(Type * type):Collection(type?type:typeObject) {
	//ctor
}

//! (dtor)
Array::~Array() {
	//dtor
}

//! (internal)
void Array::init(const ParameterValues & p){
	resize(p.count());
	size_t i=0;
	for(ParameterValues::const_iterator it=p.begin();it!=p.end();++it)
		vec[i++]=*it;
}

/*!	(internal)*/
void Array::init(size_t num,Object ** objs) {
	resize(num);
	for(size_t i=0;i<num;++i)
		vec[i]=objs[i];
}
/*!	(internal)*/
void Array::init(size_t num,char ** strings) {
	resize(num);
	for(size_t i=0;i<num;++i)
		vec[i]=String::create(strings[i]);
}

//! ---|> [Object]
Object * Array::clone()const {
	Array * newArray=new Array(getType());

	newArray->resize(count());
	size_t i=0;
	for (vector<ObjRef>::const_iterator it=vec.begin();it!=vec.end();++it)
		newArray->vec[i++]=(*it)->getRefOrCopy();
	return newArray;
}

/*!	---|> Collection*/
Object * Array::getValue(ObjPtr key) {
	if (key.isNull()) return NULL;
	size_t  index=static_cast<size_t>(key->toInt());
	if (index>=vec.size()) return NULL;
	return vec.at(index).get();
}

/*!	---|> Collection*/
void Array::setValue(ObjPtr key,ObjPtr value) {
	if (key.isNull() ) return;
	size_t index=static_cast<size_t>(key->toInt());
	if (index>=vec.size())
		vec.insert(vec.end(), index-vec.size()+1, 0);
	vec[index]=value;
}

/*!	---|> Collection*/
size_t Array::count()const {
	return vec.size();
}

//! ---|> Collection
Iterator * Array::getIterator() {
	return new ArrayIterator(this);
}

//! ---|> Collection
void Array::clear(){
	vec.clear();
}

void Array::pushBack(ObjPtr obj) {
	if (obj.isNull()) return;
	vec.push_back(obj);
}

void Array::popBack() {
	vec.pop_back();
}

void  Array::pushFront(ObjPtr obj){
	if (obj.isNull()) return;
	vec.insert(vec.begin(),obj.get());
}

void  Array::popFront(){
	vec.erase(vec.begin());
}

Object * Array::back() const{
	return vec.empty() ? NULL : (*(vec.end()-1)).get();
}

Object * Array::front()const {
	return vec.empty() ? NULL : (*(vec.begin())).get();
}

int Array::rt_indexOf(Runtime & runtime,ObjPtr search,size_t index){
	if(index>=count()||search.isNull()) return -1;

	for(vector<ObjRef>::const_iterator it=vec.begin()+index ; it!=vec.end() ; ++it) {
		if( search->isEqual(runtime,*it) )
			return index;
		++index;
	}
	return -1;
}

void Array::removeIndex(size_t index){
	 if(index>=count())
		return;
	vector<ObjRef>::iterator it=vec.begin()+index;
	vec.erase(it);
	return ;
}

void Array::reverse(){
	if(count()>1){
		size_t hSize=static_cast<size_t>(count()/2.0);
		size_t j=count()-1;
		for(size_t i=0;i<hSize;++i,--j){
			ObjRef o=vec[i];
			vec[i]=vec[j];
			vec[j]=o;
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

//! (implements quicksort)
void Array::rt_sort(Runtime & runtime,Object * function/*=NULL*/,bool reverseOrder ) {
	if (count()<=1) return;

	//quicksort(runtime,0,count()-1);
	std::stack<std::pair<size_t,size_t> > pos;
	pos.push(std::make_pair(0,count()-1));

	while (! pos.empty()) {
		size_t left=pos.top().first;
		size_t right=pos.top().second;
		pos.pop();

		// PARTITION
		//int split=partition(runtime,left,right);
		size_t split=left;
		for (size_t i=left;i<right;++i) {
			Object * di=vec[i].get();
			Object * dr=vec[right].get();

			bool change=false;

			if (di==NULL)
				change=true;
			else if (dr==NULL)
				change=false;
			else if (function!=NULL) { // comarement function given?
//				executeFunction(const ObjPtr & fun,const ObjPtr & callingObject,const ParameterValues & params,bool isConstructor=false);
				ObjRef result=callFunction(runtime,function,ParameterValues(di,dr));
				if(!runtime.assertNormalState(function))
					return;
				change=result.toBool();
			}else{
				ObjRef result=callMemberFunction(runtime,di,Consts::IDENTIFIER_fn_less,ParameterValues(dr));
				change=result.toBool();
			}

			if (change^reverseOrder) {
				vec[i].swap(vec[split]);
				split++;
			}
		}
		vec[split].swap(vec[right]);

		//--
		//quicksort(runtime,left,split-1);
		if(split > left+1) // left==split -> no elements to sort, left+1==split -> only one element
			pos.push(std::make_pair(left,split-1));

		//quicksort(runtime,split+1,right);
		if(right > split+1) // right==split -> no elements to sort, right==split+1 -> only one element
			pos.push(std::make_pair(split+1,right));
	}
}


void Array::rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues) {
	std::vector<ObjRef> tempArray;
	
	ParameterValues parameters(additionalValues.count()+1);
	if(!additionalValues.empty())
		std::copy(additionalValues.begin(),additionalValues.end(),parameters.begin()+1);

	for (vector<ObjRef>::const_iterator it=vec.begin();it!=vec.end();++it) {
		parameters.set(0,*it);
		ObjRef resultRef=callFunction(runtime,function.get(),parameters);
		if( resultRef.toBool() ){
			tempArray.push_back(*it);
		}
	}
	vec.swap(tempArray);
}

void Array::append(Collection * c){
	if(c==NULL || c==this)
		return;
	size_t i=count();
	resize( count()+c->count() );
	for(ERef<Iterator> iRef=c->getIterator(); !iRef->end() ;iRef->next()){
		ObjRef value=iRef->value();
		if(!value.isNull())
			vec[i++]=value->getRefOrCopy();
	}
}

void Array::swap(Array * other){
	vec.swap(other->vec);
}

void Array::resize(size_t newSize){
	vec.resize(newSize);
}

void Array::reserve(size_t capacity){
	vec.reserve(capacity);
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
	return end() ? NULL : Number::create(index);
}

//! ---|> [Iterator]
Object * Array::ArrayIterator::value() {
	return end() ? NULL : arrayRef->vec[index].get();
}

//! ---|> [Iterator]
void Array::ArrayIterator::next() {
	index++;
}

//! ---|> [Iterator]
void Array::ArrayIterator::reset() {
	index=0;
}

//! ---|> [Iterator]
bool Array::ArrayIterator::end() {
	return index>=arrayRef->vec.size();
}
}//namespace EScript
