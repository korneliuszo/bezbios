
enable_language(ASM)

add_library(etl_run INTERFACE)
target_include_directories(etl_run INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/etl_profile)
target_link_libraries(etl_run INTERFACE etl)


add_library(crt0 STATIC crt1.cpp)

option(CONFIG_CACHED_ASM "Use asm" ON)
if(CONFIG_CACHED_ASM)
target_sources(crt0 PRIVATE crt0.S)
else()
target_sources(crt0 PRIVATE crt0.cc)
endif()


add_library(sched STATIC sched_low.cpp)

if(CONFIG_CACHED_ASM)
target_sources(sched PRIVATE sched_asm.S)	
else()
target_sources(sched PRIVATE sched_asm.cc)	
endif()

add_library(gcc_internals STATIC stack_protector_iface.c)
target_link_libraries(crt0 gcc_internals)
set(THISDIR ${CMAKE_SOURCE_DIR}/x86-32-nofp-general-regs-only)


configure_file(thread_py.h.in thread_py.h @ONLY)
target_include_directories(sched
	PRIVATE 
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sched sched_api etl_run)

function(add_executable target_name)
	set(THISDIR ${CMAKE_SOURCE_DIR}/x86-32-nofp-general-regs-only)
	_add_executable(${target_name} ${ARGN})
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