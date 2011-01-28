#include "LocalVarMap.h"
#include "../Object.h"

#include <iostream>

using namespace EScript;

/*!	Constructor.	*/
LocalVarMap::LocalVarMap():parent(NULL),values(&m){
	//ctor
}

/*! Destructor.	*/
LocalVarMap::~LocalVarMap() {
	//dtor
}

///*!	*/
//void LocalVarMap::clearValues() {
//	while(!localVars.empty()){
//		localVars.top()->pop();
//		localVars.pop();
//	}
//}
/*!	*/
void LocalVarMap::init(LocalVarMap *_parent) {
	this->parent=_parent;
	values = parent->getValues();
}

/*!	*/
void LocalVarMap::init() {
	parent=NULL;
	values = &m;
}

/*!	*/
void  LocalVarMap::reset(){
	while(!localVars.empty()){
		localVars.top()->pop();
		localVars.pop();
	}
	parent=NULL;
	values=&m;
}

/*!	findAndUpdate	*/
bool LocalVarMap::findAndUpdate(identifierId varId,Object * val) {
	objMap_t::iterator i=getValues()->find(varId);
	if(i==getValues()->end())
		return false;
	objStack_t & s=i->second;
	if(s.empty())
		return false;
	s.top()=val;
	return true;
}
