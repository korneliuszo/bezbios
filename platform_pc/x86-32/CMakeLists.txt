set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)

project(x86-32)

add_subdirectory(multiboot)
add_subdirectory(rawdisk)

add_library(crt0 STATIC)
target_compile_options(crt0 PUBLIC -march=i486 -O2 -m32 -mgeneral-regs-only -fno-pic)
target_link_options(crt0 PUBLIC -specs=nano.specs -specs=nosys.specs)

enable_language(ASM)
target_sources(crt0 PRIVATE crtentry.S)

target_include_directories(crt0
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(crt0 PRIVATE crt0.cc)

add_library(sched STATIC sched_low.cpp)

target_sources(sched PRIVATE sched_asm.cc)
target_compile_options(sched PRIVATE -fomit-frame-pointer)
set_property(TARGET sched
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)

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
		timerPIT.cpp
)


target_sources(interrupts PRIVATE gpf.cpp)
set_source_files_properties(gpf.cpp PROPERTIES COMPILE_OPTIONS "-Wno-address-of-packed-member")

target_include_directories(interrupts
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(interrupts sched_api crt uart_api crc8)

add_library(vm86
	STATIC
)

target_sources(vm86 PRIVATE vm86.cpp)
set_source_files_properties(vm86.cpp PROPERTIES COMPILE_OPTIONS "-Wno-address-of-packed-member")
target_compile_options(vm86 PRIVATE -fomit-frame-pointer)

target_link_libraries(vm86 crt)

add_library(e820
	STATIC
		e820.cpp
)
target_link_libraries(e820 vm86 sched_api)

add_library(bios32
	STATIC
		bios32.cpp
)
target_link_libraries(bios32)

add_library(pci
	STATIC
		pci.cpp
)
target_link_libraries(pci bios32 e820 sched_api)

add_library(yenta
	STATIC
		yenta_socket.cpp
)
target_link_libraries(yenta pci sched_api)
