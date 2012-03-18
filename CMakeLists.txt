cmake_minimum_required(VERSION 2.6.4)
project(EScript CXX)

set(ESCRIPT_VERSION_MAJOR 0)
set(ESCRIPT_VERSION_MINOR 6)
set(ESCRIPT_VERSION_PATCH 0)
set(ESCRIPT_VERSION ${ESCRIPT_VERSION_MAJOR}.${ESCRIPT_VERSION_MINOR}.${ESCRIPT_VERSION_PATCH})

# Set up install directories
set(INCLUDE_INSTALL_DIR include)
set(BIN_INSTALL_DIR bin)
set(LIB_INSTALL_DIR lib)
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/EScript)

set(ESCRIPT_SOURCES
	EScript/Consts.cpp
	EScript/EScript.cpp
	EScript/Instructions/Instruction.cpp
	EScript/Instructions/InstructionBlock.cpp
	EScript/Objects/Callables/Delegate.cpp
	EScript/Objects/Callables/Function.cpp
	EScript/Objects/Callables/UserFunction.cpp
	EScript/Objects/Collections/Array.cpp
	EScript/Objects/Collections/Collection.cpp
	EScript/Objects/Collections/Map.cpp
	EScript/Objects/Exception.cpp
	EScript/Objects/ExtObject.cpp
	EScript/Objects/Iterator.cpp
	EScript/Objects/Identifier.cpp
	EScript/Objects/AST/BlockStatement.cpp
	EScript/Objects/AST/ConditionalExpr.cpp
	EScript/Objects/AST/ForeachStatement.cpp
	EScript/Objects/AST/FunctionCallExpr.cpp
	EScript/Objects/AST/GetAttributeExpr.cpp
	EScript/Objects/AST/IfStatement.cpp
	EScript/Objects/AST/LogicOpExpr.cpp
	EScript/Objects/AST/LoopStatement.cpp
	EScript/Objects/AST/SetAttributeExpr.cpp
	EScript/Objects/AST/Statement.cpp
	EScript/Objects/Namespace.cpp
	EScript/Objects/Object.cpp
	EScript/Objects/Type.cpp
	EScript/Objects/Values/Bool.cpp
	EScript/Objects/Values/Number.cpp
	EScript/Objects/Values/NumberRef.cpp
	EScript/Objects/Values/String.cpp
	EScript/Objects/Values/Void.cpp
	EScript/Objects/YieldIterator.cpp
	EScript/Parser/CompilerContext.cpp
	EScript/Parser/Operators.cpp
	EScript/Parser/Parser.cpp
	EScript/Parser/Token.cpp
	EScript/Parser/Tokenizer.cpp
	EScript/Runtime/FunctionCallContext.cpp
	EScript/Runtime/Runtime.cpp
	EScript/Runtime/RuntimeBlock.cpp
	EScript/Runtime/RuntimeContext.cpp
	EScript/Utils/AttributeContainer.cpp
	EScript/Utils/Debug.cpp
	EScript/Utils/Hashing.cpp
	EScript/Utils/Helper.cpp
	EScript/Utils/IO/DefaultFileSystemHandler.cpp
	EScript/Utils/IO/IO.cpp
	EScript/Utils/LocalVarMap.cpp
	EScript/Utils/Logger.cpp
	EScript/Utils/StringData.cpp
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

# Set version of library
set_target_properties(EScript PROPERTIES VERSION ${ESCRIPT_VERSION}
                                         SOVERSION ${ESCRIPT_VERSION_MAJOR})

# Install the header files
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Instructions/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Instructions)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Callables/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Objects/Callables)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Collections/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Objects/Collections)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/AST/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Objects/AST)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Values/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Objects/Values)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Objects)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Runtime/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Runtime)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Utils/IO/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Utils/IO)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Utils/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript/Utils)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/EScript)

# Install the library
install(TARGETS EScript EXPORT LibraryExport
	RUNTIME DESTINATION ${BIN_INSTALL_DIR}
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

# Library export
install(EXPORT LibraryExport DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE EScriptTargets.cmake)

# Set relative path of EScriptConfig.cmake to the install directory. Used in EScriptConfig.cmake.
file(RELATIVE_PATH relInstallDir ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX})

# Create the configuration files
configure_file(EScriptConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake @ONLY)
configure_file(EScriptConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake @ONLY)

# Install the configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

add_subdirectory(EScript)
add_subdirectory(tests)
