cmake_minimum_required(VERSION 2.8.8)

option(BUILD_ESCRIPT_TEST "Defines if the EScript test application is built.")
if(BUILD_ESCRIPT_TEST)
	add_executable(escript_test test.cpp)
	set_property(TARGET escript_test APPEND PROPERTY COMPILE_DEFINITIONS ES_BUILD_TEST_APPLICATION)
	target_link_libraries(escript_test EScript)
	install(TARGETS escript_test
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT tests
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
	)
	file(GLOB TEST_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/*.escript")
	install(FILES ${TEST_SCRIPTS} DESTINATION tests COMPONENT tests)

	if(COMPILER_SUPPORTS_CXX11)
		set_property(TARGET escript_test APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set_property(TARGET escript_test APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x ")
	endif()
endif()
