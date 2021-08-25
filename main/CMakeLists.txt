project(bezbios)

enable_language(ASM)


add_executable(bezbios main.cpp)
target_link_libraries(bezbios crt sched interrupts uart tlay2_uart tlay2_monitor ega tlay2_vm86_monitor)
package_floppy(bezbios)