add_library(pnp
	STATIC
		pnp.cpp
)
target_include_directories(pnp
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(pnp crt0 vm86)

add_library(pnp_monitor
	STATIC
		pnpmonitor.cpp
)

target_include_directories(pnp_monitor
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(pnp_monitor pnp tlay2) 