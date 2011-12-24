// main.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifdef ES_BUILD_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>

#include "../EScript/EScript.h"

using namespace EScript;

int main(int argc,char * argv[]) {

	if(argc<2){
		std::cout << ES_VERSION<<"\nNo filename given.\n";
		return EXIT_SUCCESS;
	}

	EScript::init();
	ERef<Runtime> rt(new Runtime());

	// --- Set program parameters
	declareConstant(rt->getGlobals(),"args",Array::create(argc,argv));

	// --- Load and execute script
	std::pair<bool,ObjRef> result = EScript::loadAndExecute(*rt.get(),argv[1]);

	// --- output result
	if (!result.second.isNull()) {
		std::cout << "\n\n --- "<<"\nResult: " << result.second.toString()<<"\n";
	}

	return result.first ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif // ES_BUILD_APPLICATION
