#!/bin/sh
gcc -c -g -Wall -Wshadow pinger.c
gcc -o pinger pinger.o
