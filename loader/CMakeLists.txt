project(loader)
enable_language(ASM)

floppy_raw(loader main.cpp)
target_link_libraries(loader crt sched interrupts uart tlay2_uart tlay2_monitor ega tlay2_vm86_monitor)

target_link_options(loader PRIVATE -Werror -Wall -Wextra -pedantic -g)
