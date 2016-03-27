#!/bin/bash
set -xe

sudo apt-get update

# essential toolchain stuff
sudo apt-get install -y build-essential nasm git mtools

# development niceties
sudo apt-get install -y vim gdb
