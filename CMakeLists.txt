cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0077 NEW)

project(bezbios)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_compile_options(-Wall -Wextra -pedantic -Werror -g -ffunction-sections -fdata-sections) 

option(CONFIG_TGTPC "Target PC" ON)

if(CONFIG_TGTPC)
	add_subdirectory(platform_pc)
endif()

add_subdirectory(crt)

add_subdirectory(sched)
add_subdirectory(uart)
add_subdirectory(display)

add_subdirectory(main)
add_subdirectory(task_low_test)
add_subdirectory(loader)

target_link_options(bezbios PRIVATE -Werror -Wall -Wextra -pedantic -g)
