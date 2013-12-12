// main.cpp
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifdef ES_BUILD_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "../EScript/EScript.h"

int main(int argc, char * argv[]) {
	EScript::init();
	EScript::ERef<EScript::Runtime> rt(new EScript::Runtime());

	// --- Set program parameters
	declareConstant(rt->getGlobals(), "args", EScript::Array::create(argc, argv));

	// --- Load and execute script
	std::pair<bool, EScript::ObjRef> result;
	if(argc == 1) {
		result = EScript::executeStream(*rt.get(), std::cin);
	} else {
		result = EScript::loadAndExecute(*rt.get(), argv[1]);
	}

	// --- output result
	if(!result.second.isNull()) {
		std::cout << "\n\n --- " << "\nResult: " << result.second.toString() << "\n";
	}

	return result.first ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif // ES_BUILD_APPLICATION
