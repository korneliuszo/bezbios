add_library(crt0 STATIC)

if(CONFIG_CACHED_ASM)
enable_language(ASM)
target_sources(crt0 PRIVATE crt0.S)
else()
target_sources(crt0 PRIVATE crt0.cc)
endif()

function(add_executable target_name)
	set(THISDIR ${CMAKE_SOURCE_DIR}/platform_pc/x86-32/multiboot)
	_add_executable(${target_name} ${ARGN})
	target_sources(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/sched/sched_main.cpp)
	target_link_options(${target_name} PUBLIC
		-m32 -nostdlib -T ${THISDIR}/multiboot.ld -static
		-Wl,-Map=$<TARGET_FILE:${target_name}>.map)
	set_target_properties(${target_name} PROPERTIES 
		LINK_DEPENDS ${THISDIR}/multiboot.ld)
	configure_file(${THISDIR}/syslinux.cfg.in syslinux.cfg @ONLY)
	add_custom_command(
		TARGET
			${target_name}
		POST_BUILD
		BYPRODUCTS
        	${target_name}.stripped
        	${target_name}.img
        	${target_name}.iso
    	COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:${target_name}>
        	-o $<TARGET_FILE:${target_name}>.stripped
    	COMMAND truncate -s 1440k
    		$<TARGET_FILE:${target_name}>.img
    	COMMAND mkfs.msdos $<TARGET_FILE:${target_name}>.img
    	COMMAND syslinux $<TARGET_FILE:${target_name}>.img
    	COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
    		${CMAKE_CURRENT_BINARY_DIR}/syslinux.cfg ::
    	COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
    		$<TARGET_FILE:${target_name}>.stripped ::
    	COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
    		/usr/share/syslinux/mboot.c32 ::
    	COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
    		/usr/share/syslinux/libcom32.c32 ::
    	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/iso
    	COMMAND cp $<TARGET_FILE:${target_name}>.img 
    		${CMAKE_CURRENT_BINARY_DIR}/iso/$<TARGET_FILE_NAME:${target_name}>.img
    	COMMAND mkisofs -o $<TARGET_FILE:${target_name}>.iso
    		-b $<TARGET_FILE_NAME:${target_name}>.img
    		-V ${target_name}
    		${CMAKE_CURRENT_BINARY_DIR}/iso
    	
)
	
endfunction()