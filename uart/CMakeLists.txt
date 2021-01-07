add_library(uart_api INTERFACE)
target_include_directories(uart_api INTERFACE ${PROJECT_SOURCE_DIR})

add_library(tlay2 STATIC tlay2.cpp crc8.c)
target_link_libraries(tlay2 sched_api)
target_include_directories(tlay2 PUBLIC ${PROJECT_SOURCE_DIR})

add_library(tlay2_uart STATIC tlay2_uart.cpp)
target_link_libraries(tlay2_uart tlay2 uart)
target_include_directories(tlay2 PUBLIC ${PROJECT_SOURCE_DIR})

add_library(tlay2_monitor STATIC monitor.cpp)
target_link_libraries(tlay2_monitor tlay2)