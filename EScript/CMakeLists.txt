cmake_minimum_required(VERSION 2.6.4)

option(BUILD_ESCRIPT_APPLICATION "Defines if the EScript application is built.")
if(BUILD_ESCRIPT_APPLICATION)
	add_executable(escript main.cpp)
	set_property(TARGET escript APPEND PROPERTY COMPILE_DEFINITIONS ES_BUILD_APPLICATION)
	target_link_libraries(escript EScript)
	install(TARGETS escript
		RUNTIME DESTINATION ${BIN_INSTALL_DIR}
		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
		ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
	)

	get_property(OLD_COMPILE_FLAGS TARGET escript PROPERTY COMPILE_FLAGS)
	if(COMPILER_SUPPORTS_CXX11)
		set_property(TARGET escript PROPERTY COMPILE_FLAGS "${OLD_COMPILE_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set_property(TARGET escript PROPERTY COMPILE_FLAGS "${OLD_COMPILE_FLAGS} -std=c++0x")
	endif()
endif()
