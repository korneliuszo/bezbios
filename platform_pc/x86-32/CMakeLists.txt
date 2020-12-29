

option(CONFIG_TGTMB "Target multiboot" ON)
if(CONFIG_TGTMB)
	add_subdirectory(multiboot)
endif()

add_library(sched STATIC sched_low.cpp)

option(CONFIG_CACHED_ASM "Use asm" ON)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(sched PRIVATE sched_asm.S)	
else()
target_sources(sched PRIVATE sched_asm.cc)	
endif()

add_library(gcc_internals STATIC stack_protector_iface.c)
set_property(TARGET gcc_internals
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)

configure_file(thread_py.h.in thread_py.h @ONLY)
target_include_directories(sched
	PRIVATE 
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sched sched_api)

add_library(interrupts
	STATIC
		interrupts.cpp
set_property(TARGET interrupts
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
target_include_directories(interrupts
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(interrupts sched_api)