#!/bin/bash

gcc -S -o sched_asm.S sched_asm.cc -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic
