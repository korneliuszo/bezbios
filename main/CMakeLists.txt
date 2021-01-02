project(bezbios)

add_executable(bezbios main.cpp)
target_link_libraries(bezbios crt sched uart interrupts)