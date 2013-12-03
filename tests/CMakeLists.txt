cmake_minimum_required(VERSION 2.8.11)

option(BUILD_ESCRIPT_TEST "Defines if the EScript test application is built.")
if(BUILD_ESCRIPT_TEST)
	add_executable(escript_test test.cpp)
	target_compile_definitions(escript_test PRIVATE ES_BUILD_TEST_APPLICATION)
	target_link_libraries(escript_test LINK_PRIVATE EScript)
	install(TARGETS escript_test
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT tests
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
	)
	# Install the EScript files needed for the tests
	install(
		DIRECTORY ./
		DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EScript/tests
		COMPONENT tests
		FILES_MATCHING PATTERN "*.escript"
	)

	if(COMPILER_SUPPORTS_CXX11)
		set_property(TARGET escript_test APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set_property(TARGET escript_test APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x ")
	endif()
endif()
