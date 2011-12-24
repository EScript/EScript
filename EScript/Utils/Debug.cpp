// Debug.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Debug.h"

#include "../Objects/Object.h"

#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace EScript;

std::map<void *,Object *> debug_objects=std::map<void *,Object *>();
int debug_objects_counter=0;

void Debug::registerObj(Object * v) {
	static int i=0;
	i++;
	if(i>1){
		//std::cout << "+"<<&debug_objects;
		debug_objects[ reinterpret_cast<void *>(v) ]=v;
		//std::cout << "+";
	}
	debug_objects_counter++;
}
void Debug::unRegisterObj(Object * v) {
	debug_objects[reinterpret_cast<void *>(v)]=0;
	debug_objects_counter--;
	//std::cout << "-"<<&debug_objects;
}
void Debug::showObjects() {
	if (debug_objects_counter==0) return;
	for ( std::map<void *,Object *>::const_iterator it = debug_objects.begin();it!=debug_objects.end();++it) {
		if ((*it).second==0)
			continue;
		//<< (void*)((*it).second)
		std::cout << " : "<< ((*it).second)<< ((*it).second)->toString();
		if (((*it).second)->getType()) std::cout << " ---|> "<<((*it).second)->getType()<<"\n";
	}
	std::cout << "\nObjects left: "<<debug_objects_counter<<"\n";
}
void Debug::clearObjects() {
	debug_objects.clear();
	debug_objects_counter=0;
}
