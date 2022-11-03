set_property(GLOBAL PROPERTY MBGF ${CMAKE_CURRENT_SOURCE_DIR})

function(floppy_grub target_name)
	get_property(THISDIR GLOBAL PROPERTY MBGF)
	
	add_executable_mboot(${target_name} ${ARGN})
	
	configure_file(${THISDIR}/menu.lst.in menu.lst @ONLY)
	add_custom_target(
			${target_name}.img ALL
		DEPENDS
			${target_name}
		BYPRODUCTS
			${target_name}.img
		COMMAND gzip -d -c ${THISDIR}/bootgrub.gz > $<TARGET_FILE:${target_name}>.img
		COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
			$<TARGET_FILE:${target_name}>.stripped ::
		COMMAND mcopy -D o -i $<TARGET_FILE:${target_name}>.img
			${CMAKE_CURRENT_BINARY_DIR}/menu.lst ::boot/grub/
		)
endfunction()