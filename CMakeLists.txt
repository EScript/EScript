cmake_minimum_required(VERSION 2.6)
project(EScript CXX)
set(ESCRIPT_SOURCES
	EScript/Consts.cpp
	EScript/EScript.cpp
	EScript/Object.cpp
	EScript/Objects/Array.cpp
	EScript/Objects/Bool.cpp
	EScript/Objects/Collection.cpp
	EScript/Objects/Delegate.cpp
	EScript/Objects/Exception.cpp
	EScript/Objects/ExtObject.cpp
	EScript/Objects/Function.cpp
	EScript/Objects/Iterator.cpp
	EScript/Objects/Map.cpp
	EScript/Objects/Namespace.cpp
	EScript/Objects/Number.cpp
	EScript/Objects/NumberRef.cpp
	EScript/Objects/String.cpp
	EScript/Objects/Type.cpp
	EScript/Objects/UserFunction.cpp
	EScript/Objects/Void.cpp
	EScript/Parser/Parser.cpp
	EScript/Parser/Operators.cpp
	EScript/Parser/Token.cpp
	EScript/Parser/Tokenizer.cpp
	EScript/Runtime/Runtime.cpp
	EScript/Runtime/RuntimeBlock.cpp
	EScript/Statements/Block.cpp
	EScript/Statements/ForControl.cpp
	EScript/Statements/ForeachControl.cpp
	EScript/Statements/FunctionCall.cpp
	EScript/Statements/GetAttribute.cpp
	EScript/Statements/Identifier.cpp
	EScript/Statements/IfControl.cpp
	EScript/Statements/LogicOp.cpp
	EScript/Statements/SetAttribute.cpp
	EScript/Statements/StateChangeControl.cpp
	EScript/Statements/TryCatchControl.cpp
	EScript/Statements/WhileControl.cpp
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
add_library(EScript ${ESCRIPT_SOURCES})

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
	add_definitions(-DES_BUILD_TEST_APPLICATION)
	add_executable(escript_test tests/test.cpp)
	target_link_libraries(escript_test EScript)
	install(TARGETS escript_test
		RUNTIME DESTINATION .
	)
	install(DIRECTORY tests
		DESTINATION .
		PATTERN ".svn" EXCLUDE
	)
endif()
