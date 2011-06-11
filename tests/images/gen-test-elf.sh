#!/bin/bash

clang -integrated-as -std=c89 -c test.c -o test-tegra2.o -ccc-host-triple armv7-none-linux-gnueabi -mcpu=cortex-a9 -mfpu=vfp
clang -integrated-as -std=c89 -c test.c -o test-arm.o -ccc-host-triple armv7-none-linux-gnueabi
clang -integrated-as -std=c89 -c test.c -o test-x86_32.o -ccc-host-triple i686-none-linux
clang -integrated-as -std=c89 -c test.c -o test-x86_64.o -ccc-host-triple x86_64-none-linux

clang -integrated-as -std=c89 -c simple-test.c -o simple-test-arm.o -ccc-host-triple armv7-none-linux-gnueabi
clang -integrated-as -std=c89 -c simple-test.c -o simple-test-x86_32.o -ccc-host-triple i686-none-linux
clang -integrated-as -std=c89 -c simple-test.c -o simple-test-x86_64.o -ccc-host-triple x86_64-none-linux
