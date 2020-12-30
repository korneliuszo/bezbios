add_library(crt STATIC crt1.cpp)
set_property(TARGET crt
             PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
target_link_libraries(crt gcc_internals crt0 interrupts uart_low)