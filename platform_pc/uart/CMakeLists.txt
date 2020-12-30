add_library(uart STATIC uart.cpp)
target_link_libraries(uart uart_api interrupts)

add_library(uart_low STATIC uart_low.cpp)
target_link_libraries(uart_low uart_api interrupts)
set_property(TARGET uart_low
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)