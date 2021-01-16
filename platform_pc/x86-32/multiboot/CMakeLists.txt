set(CONFIG_TGTMB_BOOTLOADER grub CACHE STRING "Choose bootloader")

add_subdirectory(${CONFIG_TGTMB_BOOTLOADER})

set_property(GLOBAL PROPERTY MBF ${CMAKE_CURRENT_SOURCE_DIR})

function(add_library target_name)
	_add_library(${target_name} ${ARGN})
endfunction()

function(add_executable target_name)
	get_property(THISDIR GLOBAL PROPERTY MBF)
	get_property(SCHED_MAIN GLOBAL PROPERTY SCHED_MAIN)
	_add_executable(${target_name} ${ARGN})
	target_compile_options(${target_name} INTERFACE -march=i486 -O2 -m32 -mgeneral-regs-only -nostdinc -fno-pic)
	target_sources(${target_name} PRIVATE ${SCHED_MAIN})
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

add_library(crt0 STATIC)
target_compile_options(crt0 PUBLIC -march=i486 -O2 -m32 -mgeneral-regs-only -nostdinc -fno-pic)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(crt0 PRIVATE crt0.S)
else()
target_sources(crt0 PRIVATE crt0.cc)
endif()