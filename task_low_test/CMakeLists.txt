project(task_low_test)
enable_language(ASM)

floppy_grub(task_low_test main.cpp)
target_link_libraries(task_low_test sched uart_low crt)