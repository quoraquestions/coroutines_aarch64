aarch64-linux-gnu-as --gstabs -o context.o context.s
aarch64-linux-gnu-objdump -Ds ./context.o > context.dis
aarch64-linux-gnu-gcc thr.c context.o -o thr -Wall -ggdb3 -O0 -pedantic -Wundef -Wextra -fno-inline  -fno-omit-frame-pointer -ffunction-sections -fpie
objdump -aS thr > thr.dis
