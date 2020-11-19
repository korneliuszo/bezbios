cmake_minimum_required(VERSION 3.5.0)
cmake_policy(SET CMP0077 NEW)

project(bezbios)

add_compile_options(-Wall -Wextra -pedantic -Werror -g) 

add_subdirectory(etl)

add_subdirectory(sched)
add_subdirectory(uart)

option(CONFIG_M32NOFPGRO "Target m32" ON)
if(CONFIG_M32NOFPGRO)
	include(x86-32-nofp-general-regs-only/Prologue.txt)
	add_subdirectory(x86-32-nofp-general-regs-only)
endif()

option(CONFIG_TGTPC "Target PC" ON)
if(CONFIG_TGTPC)
	add_subdirectory(platform_pc)
endif()


add_subdirectory(main)
add_subdirectory(task_low_test)

target_link_options(bezbios PRIVATE -Werror -Wall -Wextra -pedantic -g)
