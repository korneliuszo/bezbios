project(bezbios)

add_executable(bezbios main.cpp)
target_link_libraries(bezbios crt sched interrupts uart tlay2_uart tlay2_monitor ega)
package_floppy(bezbios)