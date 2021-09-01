add_library(crt STATIC crt1.cpp)
target_link_libraries(crt gcc_internals crt0 uart_low)
