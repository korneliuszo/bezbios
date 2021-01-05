add_library(crt0 STATIC)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(crt0 PRIVATE crt0.S)
else()
target_sources(crt0 PRIVATE crt0.cc)
endif()

set(CONFIG_TGTMB_BOOTLOADER syslinux CACHE STRING "Choose bootloader")

add_subdirectory(${CONFIG_TGTMB_BOOTLOADER})

function(add_executable target_name)
	set(THISDIR ${CMAKE_SOURCE_DIR}/platform_pc/x86-32/multiboot)
	_add_executable(${target_name} ${ARGN})
	target_sources(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/sched/sched_main.cpp)
	target_link_options(${target_name} PUBLIC
		-m32 -nostdlib -T ${THISDIR}/multiboot.ld -static
		-Wl,-Map=$<TARGET_FILE:${target_name}>.map)
	set_target_properties(${target_name} PROPERTIES 
		LINK_DEPENDS ${THISDIR}/multiboot.ld)
	add_custom_command(
		TARGET
			${target_name}
		POST_BUILD
		BYPRODUCTS
			${target_name}.lss
        	${target_name}.stripped
    	COMMAND ${CMAKE_OBJDUMP} -h -S
    		$<TARGET_FILE:${target_name}> | c++filt > $<TARGET_FILE:${target_name}>.lss
    	COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:${target_name}>
        	-o $<TARGET_FILE:${target_name}>.stripped
    	
)
	
endfunction()