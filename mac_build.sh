#!/bin/sh
make clean
hg pull
hg update -v
qmake imageshack.pro -spec macx-g++
make
