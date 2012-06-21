cmake_minimum_required(VERSION 2.8.8)

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
endif()
