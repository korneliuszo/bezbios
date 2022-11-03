add_subdirectory(grub)
add_subdirectory(syslinux)

set_property(GLOBAL PROPERTY MBF ${CMAKE_CURRENT_SOURCE_DIR})

function(add_executable_mboot target_name)
	get_property(THISDIR GLOBAL PROPERTY MBF)
	get_property(SCHED_MAIN GLOBAL PROPERTY SCHED_MAIN)
	add_executable(${target_name} ${ARGN})
	target_compile_options(${target_name} INTERFACE -march=i486 -O2 -m32 -mgeneral-regs-only -nostdinc -fno-pic)
	target_sources(${target_name} PRIVATE ${SCHED_MAIN})
	target_sources(${target_name} PRIVATE ${THISDIR}/multiboot_header.cpp)
	target_link_options(${target_name} PUBLIC
		-m32 -nostartfiles -T ${THISDIR}/multiboot.ld -static
		-Wl,--gc-sections
		-Wl,--undefined=_cstart
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
