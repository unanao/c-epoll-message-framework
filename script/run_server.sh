#!/bin/sh

PROJECT_ROOT="`pwd`/.."

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROJECT_ROOT/lib/:$PROJECT_ROOT/debug_lib/

$PROJECT_ROOT/server/netd
