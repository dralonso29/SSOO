#!/bin/bash
mkdir /tmp/a
echo aaa> /tmp/a/nada
dd if=/dev/zero of=/tmp/a/megas bs=10M count=1
dd if=/dev/zero of=/tmp/a/diez bs=10 count=1
echo bbbbb >> /tmp/a/diez
