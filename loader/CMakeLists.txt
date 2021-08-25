project(loader)
enable_language(ASM)

add_executable(loader main.cpp)
target_link_libraries(loader crt sched interrupts uart tlay2_uart tlay2_monitor ega tlay2_vm86_monitor)
package_floppy(loader)

target_link_options(loader PRIVATE -Werror -Wall -Wextra -pedantic -g)
