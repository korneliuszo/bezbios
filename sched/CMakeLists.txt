add_library(sched_api STATIC sched_process.cpp sched_mutex.cpp)
target_include_directories(sched_api PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(sched_api interrupts)

set_property(GLOBAL PROPERTY SCHED_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/sched_main.cpp)