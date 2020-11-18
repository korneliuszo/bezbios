add_library(uart STATIC uart.c)
target_link_libraries(uart uart_api)