// test.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifdef ES_BUILD_TEST_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>

#include "../EScript/EScript.h"

#ifdef ES_DEBUG_MEMORY
#include "../EScript/Parser/Tokenizer.h"
#include "../EScript/Utils/Debug.h"
#endif

using namespace EScript;


int main(int argc,char * argv[]) {

	EScript::init();

#ifdef ES_DEBUG_MEMORY
	Tokenizer::identifyStaticToken(0); // init constants
	Debug::clearObjects();
#endif

	ERef<Runtime> rt(new Runtime());

	declareConstant(rt->getGlobals(),"args",Array::create(argc,argv));

	// --- Test NumberRef
	int numberRefTest1=17;
	declareConstant(rt->getGlobals(),"numberRefTest1",new NumberRef(numberRefTest1));
	float numberRefTest2=17;
	declareConstant(rt->getGlobals(),"numberRefTest2",new NumberRef(numberRefTest2));

	// --- Load and execute script
	std::string file= argc>1 ? argv[1] : "tests/test.escript";
	std::pair<bool,ObjRef> result = EScript::loadAndExecute(*rt.get(),file);


//    // --- Test NumberRef
//    std::cout << numberRefTest1<<"," <<numberRefTest2; // should be (18,16)

	// --- output result
	if (!result.second.isNull()) {
		std::cout << "\n\n --- "<<"\nResult: " << result.second.toString()<<"\n";
	}

	// --- cleanup
	result.second=NULL;
	rt=NULL;

#ifdef ES_DEBUG_MEMORY
	Debug::showObjects();
#endif
	return result.first ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif // ES_BUILD_TEST_APPLICATION
