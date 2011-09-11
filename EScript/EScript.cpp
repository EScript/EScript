// EScript.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "EScript.h"
#include "Objects/Identifier.h"
#include "Objects/YieldIterator.h"
#include "Objects/Streams/StreamBase.h"
#include "Parser/Parser.h"

#include "../E_Libs/StdLib.h"
#ifdef _WIN32
#include "../E_Libs/Win32Lib.h"
#endif
#include "../E_Libs/IOLib.h"
#include "../E_Libs/MathLib.h"

namespace EScript {

//! (static)
void init() {
	Namespace * SGLOBALS=getSGlobals();

	Object::init(*SGLOBALS);
	Type::init(*SGLOBALS);
	ExtObject::init(*SGLOBALS);

	Number::init(*SGLOBALS);
	NumberRef::init(*SGLOBALS);
	Bool::init(*SGLOBALS);
	String::init(*SGLOBALS);

	Collection::init(*SGLOBALS);
	Identifier::init(*SGLOBALS);
	Iterator::init(*SGLOBALS);
	Array::init(*SGLOBALS);
	Map::init(*SGLOBALS);
	Exception::init(*SGLOBALS);
	Delegate::init(*SGLOBALS);
	Namespace::init(*SGLOBALS);
	Function::init(*SGLOBALS);
	UserFunction::init(*SGLOBALS);
	YieldIterator::init(*SGLOBALS);
	StreamBase::init(*SGLOBALS);

	declareConstant(SGLOBALS,"VERSION",String::create(ES_VERSION));
	declareConstant(SGLOBALS,"SGLOBALS",SGLOBALS);

	Parser::init(*SGLOBALS);
	Runtime::init(*SGLOBALS);

	initLibrary(StdLib::init);
	initLibrary(IOLib::init);
	initLibrary(MathLib::init);
	#ifdef _WIN32
	initLibrary(Win32Lib::init);
	#endif
}

//! (static,internal)
 Namespace * getSGlobals() {
	static ERef<Namespace> sglobals(new Namespace);
	return sglobals.get();
}

//! (static)
void initLibrary(libInitFunction * initFunction) {
	(*initFunction)(getSGlobals());
}

}
