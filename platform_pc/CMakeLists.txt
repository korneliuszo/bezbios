option(CONFIG_TGT32 "Target x86-32" ON)

if(CONFIG_TGT32)
	add_subdirectory(x86-32)
endif()

project(platform_pc)
add_subdirectory(uart)
add_subdirectory(ega)
add_subdirectory(apm)
add_subdirectory(pnp)

