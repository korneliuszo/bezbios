function(package_floppy target_name)
	set(THISDIR ${CMAKE_SOURCE_DIR}/platform_pc/x86-32/multiboot/syslinux)
	configure_file(${THISDIR}/syslinux.cfg.in syslinux.cfg @ONLY)
	add_custom_target(
			${target_name}.img ALL
		DEPENDS
			${target_name}
		BYPRODUCTS
			${target_name}.img
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
		)
endfunction()