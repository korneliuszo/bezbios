add_library(ega STATIC ega.cpp)
target_link_libraries(ega display_api sched_api interrupts)
set_property(TARGET ega PROPERTY CXX_STANDARD 20)