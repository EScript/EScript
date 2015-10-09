# CMakeLists.txt
# This file is part of the EScript programming language (https://github.com/EScript)
#
# Copyright (C) 2012-2015 Benjamin Eikel <benjamin@eikel.org>
#
# Licensed under the MIT License. See LICENSE file for details.
# ---------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1.0)

option(BUILD_ESCRIPT_APPLICATION "Defines if the EScript application is built.")
if(BUILD_ESCRIPT_APPLICATION)
	add_executable(escript main.cpp)
	target_compile_definitions(escript PRIVATE ES_BUILD_APPLICATION)
	target_link_libraries(escript LINK_PRIVATE EScript)
	install(TARGETS escript
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT applications
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT applications
	)
endif()
