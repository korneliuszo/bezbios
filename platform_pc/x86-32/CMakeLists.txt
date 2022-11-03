option(CONFIG_CACHED_ASM "Use asm" OFF)

option(CONFIG_TGTMB "Target multiboot" ON)
if(CONFIG_TGTMB)
	add_subdirectory(multiboot)
endif()

option(CONFIG_TGTRD "Target rawdisk" OFF)
if(CONFIG_TGTRD)
	add_subdirectory(rawdisk)
endif()

add_library(crt0 STATIC)
target_compile_options(crt0 PUBLIC -march=i486 -O2 -m32 -mgeneral-regs-only -fno-pic)
target_link_options(crt0 PUBLIC -specs=nano.specs -specs=nosys.specs)

enable_language(ASM)
target_sources(crt0 PRIVATE crtentry.S)

target_include_directories(crt0
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}
)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(crt0 PRIVATE crt0.S)
else()
target_sources(crt0 PRIVATE crt0.cc)
endif()

add_library(sched STATIC sched_low.cpp)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(sched PRIVATE sched_asm.S)	
else()
target_sources(sched PRIVATE sched_asm.cc)
target_compile_options(sched PRIVATE -fomit-frame-pointer)
set_property(TARGET sched
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
endif()

add_library(gcc_internals STATIC cxa_guard.cpp)
set_property(TARGET gcc_internals
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
target_link_libraries(gcc_internals crt0 sched)

configure_file(thread_py.h.in thread_py.h @ONLY)
target_include_directories(sched
	PRIVATE 
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sched sched_api crt)

add_library(interrupts
	STATIC
		interrupts.cpp
		interrupts_default.cpp
		timerPIT.cpp
)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(interrupts PRIVATE gpf.S)
else()
target_sources(interrupts PRIVATE gpf.cpp)
set_source_files_properties(gpf.cpp PROPERTIES COMPILE_OPTIONS "-Wno-address-of-packed-member")
endif()

target_include_directories(interrupts
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(interrupts sched_api crt uart_api crc8)

add_library(vm86
	STATIC
)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(vm86 PRIVATE vm86.S)
else()
target_sources(vm86 PRIVATE vm86.cpp)
set_source_files_properties(vm86.cpp PROPERTIES COMPILE_OPTIONS "-Wno-address-of-packed-member")
target_compile_options(vm86 PRIVATE -fomit-frame-pointer)
endif()

target_link_libraries(vm86 crt)
