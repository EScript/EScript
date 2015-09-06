# CMakeLists.txt
# This file is part of the EScript programming language (https://github.com/EScript)
#
# Copyright (C) 2011-2014 Claudius Jähn <ClaudiusJ@live.de>
# Copyright (C) 2011-2015 Benjamin Eikel <benjamin@eikel.org>
#
# Licensed under the MIT License. See LICENSE file for details.
# ---------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.11)
project(EScript CXX)

set(ESCRIPT_VERSION_MAJOR 0)
set(ESCRIPT_VERSION_MINOR 7)
set(ESCRIPT_VERSION_PATCH 2)
set(ESCRIPT_VERSION_STRING ${ESCRIPT_VERSION_MAJOR}.${ESCRIPT_VERSION_MINOR}.${ESCRIPT_VERSION_PATCH})

# Set up install directories
include(GNUInstallDirs)
set(CMAKE_INSTALL_CMAKECONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/EScript)

set(ESCRIPT_SOURCES
	EScript/Compiler/AST/UserFunctionExpr.cpp
	EScript/Compiler/Compiler.cpp
	EScript/Compiler/FnCompileContext.cpp
	EScript/Compiler/Operators.cpp
	EScript/Compiler/Parser.cpp
	EScript/Compiler/Token.cpp
	EScript/Compiler/Tokenizer.cpp
	EScript/Consts.cpp
	EScript/EScript.cpp
	EScript/Instructions/InstructionBlock.cpp
	EScript/Instructions/Instruction.cpp
	EScript/Objects/Callables/FnBinder.cpp
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
	EScript/Objects/Values/String.cpp
	EScript/Objects/Values/Void.cpp
	EScript/Objects/YieldIterator.cpp
	EScript/Runtime/FunctionCallContext.cpp
	EScript/Runtime/RtValue.cpp
	EScript/Runtime/Runtime.cpp
	EScript/Runtime/RuntimeInternals.cpp
	EScript/Utils/AttributeContainer.cpp
	EScript/Utils/Debug.cpp
	EScript/Utils/DeclarationHelper.cpp
	EScript/Utils/Hashing.cpp
	EScript/Utils/RuntimeHelper.cpp
	EScript/Utils/IO/DefaultFileSystemHandler.cpp
	EScript/Utils/IO/IO.cpp
	EScript/Utils/Logger.cpp
	EScript/Utils/StdConversions.cpp
	EScript/Utils/StdFactories.cpp
	EScript/Utils/StringData.cpp
	EScript/Utils/StringUtils.cpp
	E_Libs/ext/JSON.cpp
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
set_target_properties(EScript PROPERTIES VERSION ${ESCRIPT_VERSION_STRING}
										 SOVERSION ${ESCRIPT_VERSION_MAJOR})

# Install the header files
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Instructions/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Instructions COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Callables/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Objects/Callables COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Collections/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Objects/Collections COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/AST/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Objects/AST COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/Values/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Objects/Values COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Objects/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Objects COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Runtime/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Runtime COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Utils/IO/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Utils/IO COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/Utils/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript/Utils COMPONENT headers)
file(GLOB ESCRIPT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/EScript/*.h")
install(FILES ${ESCRIPT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EScript COMPONENT headers)

# Install the EScript standard library
install(
	DIRECTORY Std
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EScript
	COMPONENT runtimelibraries
	FILES_MATCHING PATTERN "*.escript"
)

# Install the library
install(TARGETS EScript EXPORT LibraryExport
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtimelibraries
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtimelibraries
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT developmentlibraries
)

# Set include directories for users of this library
target_include_directories(EScript
						   INTERFACE
						   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
						   "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

# Library export
install(EXPORT LibraryExport DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR} FILE EScriptTargets.cmake COMPONENT developmentlibraries)

# Create the configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(EScriptConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake
							  INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR}
							  PATH_VARS CMAKE_INSTALL_CMAKECONFIGDIR
							  NO_SET_AND_CHECK_MACRO
							  NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake
								 VERSION ${ESCRIPT_VERSION_STRING}
								 COMPATIBILITY SameMajorVersion)

# Install the configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/EScriptConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR} COMPONENT developmentlibraries)

add_subdirectory(EScript)

option(BUILD_ESCRIPT_TEST "Defines if the EScript test application is built.")
if(BUILD_ESCRIPT_TEST)
	if(UNIX AND NOT APPLE)
		find_program(MEMORYCHECK_COMMAND NAMES valgrind)
		set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=summary --num-callers=1 --vgdb=no")
	endif()
	include(CTest)
	add_subdirectory(tests)
endif()

# Packaging
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "libescript${ESCRIPT_VERSION_MAJOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EScript library")
set(CPACK_PACKAGE_VENDOR "Benjamin Eikel; Claudius Jaehn; Ralf Petring")
set(CPACK_PACKAGE_CONTACT "Benjamin Eikel <benjamin@eikel.org>")
set(CPACK_PACKAGE_VERSION_MAJOR ${ESCRIPT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ESCRIPT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ESCRIPT_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "libEScript")
set(CPACK_STRIP_FILES ON)

set(CPACK_COMPONENTS_ALL applications developmentlibraries headers runtimelibraries tests)
set(CPACK_COMPONENT_APPLICATIONS_DEPENDS runtimelibraries)
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
set(CPACK_COMPONENT_DEVELOPMENTLIBRARIES_GROUP "Development")
set(CPACK_COMPONENT_HEADERS_DEPENDS developmentlibraries)
set(CPACK_COMPONENT_HEADERS_GROUP "Development")
set(CPACK_COMPONENT_RUNTIMELIBRARIES_GROUP "Runtime")
set(CPACK_COMPONENT_TESTS_DEPENDS runtimelibraries)
set(CPACK_COMPONENT_TESTS_GROUP "Runtime")

include(CPack)
