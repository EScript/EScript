cmake_minimum_required(VERSION 2.8.8)
project(EScript CXX)

set(ESCRIPT_VERSION_MAJOR 0)
set(ESCRIPT_VERSION_MINOR 6)
set(ESCRIPT_VERSION_PATCH 3)
set(ESCRIPT_VERSION ${ESCRIPT_VERSION_MAJOR}.${ESCRIPT_VERSION_MINOR}.${ESCRIPT_VERSION_PATCH})

message(WARNING "Experimental EScript version ${ESCRIPT_VERSION}")

# Set up install directories
set(INCLUDE_INSTALL_DIR include)
set(BIN_INSTALL_DIR bin)
set(LIB_INSTALL_DIR lib)
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/EScript)

set(ESCRIPT_SOURCES
	EScript/Consts.cpp
	EScript/EScript.cpp
	EScript/Instructions/InstructionBlock.cpp
	EScript/Instructions/Instruction.cpp
	EScript/Objects/AST/UserFunctionExpr.cpp
	EScript/Objects/Callables/Delegate.cpp
	EScript/Objects/Callables/Function.cpp
	EScript/Objects/Callables/UserFunction.cpp
	EScript/Objects/Collections/Array.cpp
	EScript/Objects/Collections/Collection.cpp
	EScript/Objects/Collections/Map.cpp
	EScript/Objects/Exception.cpp
	EScript/Objects/ExtObject.cpp
	EScript/Objects/Identifier.cpp
	EScript/Objects/Iterator.cpp
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
	EScript/Parser/Compiler.cpp
	EScript/Parser/Operators.cpp
	EScript/Parser/Parser.cpp
	EScript/Parser/Token.cpp
	EScript/Parser/Tokenizer.cpp
	EScript/Runtime/FunctionCallContext.cpp
	EScript/Runtime/Runtime.cpp
	EScript/Runtime/RuntimeInternals.cpp
	EScript/Utils/AttributeContainer.cpp
	EScript/Utils/Debug.cpp
	EScript/Utils/Hashing.cpp
	EScript/Utils/Helper.cpp
	EScript/Utils/IO/DefaultFileSystemHandler.cpp
	EScript/Utils/IO/IO.cpp
	EScript/Utils/Logger.cpp
	EScript/Utils/StringData.cpp
	EScript/Utils/StringUtils.cpp
	E_Libs/ext/JSON.cpp
	E_Libs/ext/Rand.cpp
	E_Libs/IOLib.cpp
	E_Libs/MathLib.cpp
	E_Libs/StdLib.cpp
)
if(WIN32)
	list(APPEND ESCRIPT_SOURCES E_Libs/Win32Lib.cpp)
endif()
add_library(EScript SHARED ${ESCRIPT_SOURCES})

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set_property(TARGET EScript APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
elseif(COMPILER_SUPPORTS_CXX0X)
	set_property(TARGET EScript APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x ")
else()
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

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

# Create the configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(EScriptConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS INCLUDE_INSTALL_DIR CMAKECONFIG_INSTALL_DIR
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake
                                 VERSION ${ESCRIPT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

# Install the configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

add_subdirectory(EScript)
add_subdirectory(tests)
