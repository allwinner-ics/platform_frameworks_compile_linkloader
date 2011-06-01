#!/bin/bash

clang -integrated-as -c test.c -o test-arm.o -ccc-host-triple armv7-none-linux-gnueabi -std=c89
clang -integrated-as -c simple-test.c -o simple-test-arm.o -ccc-host-triple armv7-none-linux-gnueabi -std=c89
clang -integrated-as -c test.c -o test-x86_32.o -ccc-host-triple i686-none-linux -std=c89
clang -integrated-as -c test.c -o test-x86_64.o -ccc-host-triple x86_64-none-linux -std=c89
