project(bezbios_pcmcia)

enable_language(ASM)


floppy_grub(pcmcia_test main.cpp)
target_link_libraries(pcmcia_test
	crt
	sched
	interrupts
	uart
	tlay2_uart
	tlay2_monitor
	tlay2_vm86_monitor
	apm
	yenta
)
