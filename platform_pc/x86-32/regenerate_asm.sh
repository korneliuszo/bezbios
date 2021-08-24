#!/bin/bash

gcc -S -o sched_asm.S sched_asm.cc -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic
gcc -S -o vm86.S vm86.cpp -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic -I ../../
gcc -S -o gpf.S gpf.cpp -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic -I ../../
#!/bin/bash

gcc -S -o crt0.S crt0.cc -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic