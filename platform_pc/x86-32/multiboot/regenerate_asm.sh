#!/bin/bash

gcc -S -o crt0.S crt0.cc -march=i486 -mgeneral-regs-only -m32 -O2 -nostdlib -fno-pic