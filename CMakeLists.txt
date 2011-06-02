cmake_minimum_required(VERSION 2.6)
project(EScript CXX)
set(ESCRIPT_SOURCES
	EScript/Consts.cpp
	EScript/EScript.cpp
	EScript/Objects/Array.cpp
	EScript/Objects/Bool.cpp
	EScript/Objects/Collection.cpp
	EScript/Objects/Delegate.cpp
	EScript/Objects/Exception.cpp
	EScript/Objects/ExtObject.cpp
	EScript/Objects/Function.cpp
	EScript/Objects/Iterator.cpp
	EScript/Objects/Identifier.cpp
	EScript/Objects/Internals/Block.cpp
	EScript/Objects/Internals/ConditionalExpr.cpp
	EScript/Objects/Internals/FunctionCall.cpp
	EScript/Objects/Internals/GetAttribute.cpp
	EScript/Objects/Internals/IfControl.cpp
	EScript/Objects/Internals/LogicOp.cpp
	EScript/Objects/Internals/SetAttribute.cpp
	EScript/Objects/Internals/Statement.cpp
	EScript/Objects/Map.cpp
	EScript/Objects/Namespace.cpp
	EScript/Objects/Number.cpp
	EScript/Objects/NumberRef.cpp
	EScript/Objects/Object.cpp
	EScript/Objects/String.cpp
	EScript/Objects/Type.cpp
	EScript/Objects/UserFunction.cpp
	EScript/Objects/Void.cpp
	EScript/Objects/YieldIterator.cpp
	EScript/Parser/Parser.cpp
	EScript/Parser/Operators.cpp
	EScript/Parser/Token.cpp
	EScript/Parser/Tokenizer.cpp
	EScript/Runtime/Runtime.cpp
	EScript/Runtime/RuntimeBlock.cpp
	EScript/Runtime/RuntimeContext.cpp
	EScript/Utils/Debug.cpp
	EScript/Utils/FileUtils.cpp
	EScript/Utils/Hashing.cpp
	EScript/Utils/LocalVarMap.cpp
	EScript/Utils/StringUtils.cpp
	E_Libs/ext/Rand.cpp
	E_Libs/ext/JSON.cpp
	E_Libs/IOLib.cpp
	E_Libs/StdLib.cpp
	E_Libs/MathLib.cpp
)
if(WIN32)
	list(APPEND ESCRIPT_SOURCES E_Libs/Win32Lib.cpp)
endif()
add_library(EScript SHARED ${ESCRIPT_SOURCES})

install(TARGETS EScript
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(DIRECTORY EScript
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
)

option(BUILD_ESCRIPT_TEST "Defines if the EScript test application is built.")
if(BUILD_ESCRIPT_TEST)
	add_executable(escript_test tests/test.cpp)
	set_property(TARGET escript_test APPEND PROPERTY COMPILE_DEFINITIONS "ES_BUILD_TEST_APPLICATION")
	target_link_libraries(escript_test EScript)
	install(TARGETS escript_test
		RUNTIME DESTINATION .
	)
	install(DIRECTORY tests
		DESTINATION .
		PATTERN ".svn" EXCLUDE
	)
endif()
