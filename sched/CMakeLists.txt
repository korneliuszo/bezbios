add_library(sched_api STATIC sched_process.cpp sched_mutex.cpp)
target_include_directories(sched_api PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(sched_api interrupts)