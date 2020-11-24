add_library(sched_api STATIC sched_process.cpp)
target_include_directories(sched_api PUBLIC ${PROJECT_SOURCE_DIR})