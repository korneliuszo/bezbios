cmake_minimum_required(VERSION 3.17.0)
cmake_policy(SET CMP0077 NEW)


add_compile_options(-Wall -Wextra -pedantic -Werror -g -ffunction-sections -fdata-sections -fno-exceptions) 

option(CONFIG_TGTPC "Target PC" ON)

if(CONFIG_TGTPC)
	add_subdirectory(platform_pc)
endif()
project(bezbios)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
add_subdirectory(crt)

add_subdirectory(sched)
add_subdirectory(uart)
add_subdirectory(display)


if(PROJECT_IS_TOP_LEVEL)
add_subdirectory(main)
add_subdirectory(task_low_test)
add_subdirectory(loader)
endif()
