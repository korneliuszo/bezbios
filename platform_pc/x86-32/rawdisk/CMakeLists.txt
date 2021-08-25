set_property(GLOBAL PROPERTY RDF ${CMAKE_CURRENT_SOURCE_DIR})

function(add_library target_name)
	_add_library(${target_name} ${ARGN})
endfunction()

function(add_executable target_name)
	get_property(THISDIR GLOBAL PROPERTY RDF)
	get_property(SCHED_MAIN GLOBAL PROPERTY SCHED_MAIN)
	_add_executable(${target_name} ${ARGN})
	target_compile_options(${target_name} INTERFACE -march=i486 -O2 -m32 -mgeneral-regs-only -nostdinc -fno-pic)
	target_sources(${target_name} PRIVATE ${SCHED_MAIN})
	target_link_options(${target_name} PUBLIC
		-m32 -nostdlib -T ${THISDIR}/rawdisk.ld -static
		-Wl,-Map=$<TARGET_FILE:${target_name}>.map)
	set_target_properties(${target_name} PROPERTIES 
		LINK_DEPENDS ${THISDIR}/rawdisk.ld)
	add_custom_command(
		TARGET
			${target_name}
		POST_BUILD
		BYPRODUCTS
			${target_name}.lss
        	${target_name}.stripped
        	${target_name}.bin
    	COMMAND ${CMAKE_OBJDUMP} -h -S
    		$<TARGET_FILE:${target_name}> | c++filt > $<TARGET_FILE:${target_name}>.lss
    	COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:${target_name}>
        	-o $<TARGET_FILE:${target_name}>.stripped
    	COMMAND ${CMAKE_OBJCOPY} -O binary -v
 	   		-j .text
    		-j .rodata
    		-j .data
    		-j .loram_code
      		$<TARGET_FILE:${target_name}>  $<TARGET_FILE:${target_name}>.bin
 	)
	
endfunction()


function(package_floppy target_name)
	get_property(THISDIR GLOBAL PROPERTY RDF)

	_add_executable(${target_name}_mbr ${THISDIR}/mbr.S)
	target_compile_options(${target_name}_mbr PRIVATE 
		-march=i486 -O2 -m16 -mgeneral-regs-only 
		-nostdinc -fno-pic -x assembler-with-cpp
		-D IMAGE=$<TARGET_FILE:${target_name}>.bin
	)
	target_link_options(${target_name}_mbr PUBLIC
		-m16 -nostdlib -static
		-T ${THISDIR}/mbr.ld
		-Wl,-Map=$<TARGET_FILE:${target_name}_mbr>.map)
	add_dependencies(${target_name}_mbr ${target_name})
		set_target_properties(${target_name} PROPERTIES 
		LINK_DEPENDS ${THISDIR}/mbr.ld)
	add_custom_command(
		TARGET
			${target_name}_mbr
    	COMMAND ${CMAKE_OBJDUMP} -h -S -M i8086
    		$<TARGET_FILE:${target_name}_mbr> | c++filt > $<TARGET_FILE:${target_name}_mbr>.lss
		COMMAND ${CMAKE_OBJCOPY} -O binary -v -j .text --change-section-lma .text=0
    		$<TARGET_FILE:${target_name}_mbr>  $<TARGET_FILE:${target_name}_mbr>.bin
	)

	add_custom_target(
			${target_name}.img ALL
		DEPENDS
			${target_name}_mbr
		BYPRODUCTS
			${target_name}.img
		COMMAND dd if=$<TARGET_FILE:${target_name}_mbr>.bin of=$<TARGET_FILE:${target_name}>.img
		COMMAND truncate -s 1440k
			$<TARGET_FILE:${target_name}>.img
	)
endfunction()