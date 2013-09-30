cmake_minimum_required(VERSION 2.8.11)

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

	if(COMPILER_SUPPORTS_CXX11)
		set_property(TARGET escript APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set_property(TARGET escript APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x ")
	endif()
endif()
