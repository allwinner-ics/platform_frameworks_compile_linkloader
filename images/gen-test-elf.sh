#!/bin/bash

clang -integrated-as -c test.c -o test-arm.o -ccc-host-triple armv7-none-linux-gnueabi
clang -integrated-as -c test.c -o test-x86_32.o -ccc-host-triple i686-none-linux
clang -integrated-as -c test.c -o test-x86_64.o -ccc-host-triple x86_64-none-linux
