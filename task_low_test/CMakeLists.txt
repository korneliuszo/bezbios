project(bezbios)

add_executable(task_low_test main.cpp)
target_link_libraries(task_low_test crt sched uart)