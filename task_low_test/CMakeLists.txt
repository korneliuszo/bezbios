project(bezbios)

add_executable(task_low_test main.cpp)
target_link_libraries(task_low_test sched uart_low crt)
package_floppy(task_low_test)