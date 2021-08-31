add_library(uart_api INTERFACE)
target_include_directories(uart_api INTERFACE ${PROJECT_SOURCE_DIR})

add_library(crc8 STATIC crc8.c)
target_include_directories(crc8 PUBLIC ${PROJECT_SOURCE_DIR})
set_property(TARGET crc8
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
target_link_libraries(crc8 crt0)

add_library(tlay2 STATIC tlay2.cpp)
target_link_libraries(tlay2 sched_api crt crc8)
target_include_directories(tlay2 PUBLIC ${PROJECT_SOURCE_DIR})

add_library(tlay2_uart STATIC tlay2_uart.cpp)
target_link_libraries(tlay2_uart tlay2 uart)
target_include_directories(tlay2 PUBLIC ${PROJECT_SOURCE_DIR})

add_library(tlay2_monitor STATIC monitor.cpp)
target_link_libraries(tlay2_monitor tlay2)

add_library(tlay2_vm86_monitor STATIC vm86_monitor.cpp)
target_link_libraries(tlay2_vm86_monitor tlay2 vm86)