
function(floppy_grub target_name)

	add_executable_mboot(${target_name} ${ARGN})
	
	configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/menu.lst.in menu.lst @ONLY)
	add_custom_target(
			${target_name}.img ALL
		DEPENDS
			${target_name}.stripped
		BYPRODUCTS
			${target_name}.img
		COMMAND gzip -d -c ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/bootgrub.gz > $<TARGET_FILE:${target_name}>.img
		COMMAND mcopy -i $<TARGET_FILE:${target_name}>.img
			$<TARGET_FILE:${target_name}>.stripped ::
		COMMAND mcopy -D o -i $<TARGET_FILE:${target_name}>.img
			${CMAKE_CURRENT_BINARY_DIR}/menu.lst ::boot/grub/
		)
endfunction()