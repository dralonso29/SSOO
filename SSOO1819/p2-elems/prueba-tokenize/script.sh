#!/bin/sh
gcc -c -g -Wall -Wshadow token.c
gcc -o token token.o
