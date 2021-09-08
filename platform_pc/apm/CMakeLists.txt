add_library(apm
	STATIC
		apm.cpp
)
target_include_directories(apm
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(apm vm86)
