#!/bin/sh

LD_PRELOAD=./libexray.so
LD_LIBRARY_PATH=$PWD
LIBEXRAY=logfile=exray-preloadtest.log

export LD_LIBRARY_PATH LD_PRELOAD LIBEXRAY

./preloadtest
